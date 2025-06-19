// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "boxhandler/data/boxdata.h"
#include "jsonhandler/jsonhandler.h"
#include "boxhandler/boxhandler.h"

// Sets default values
Ajsonhandler::Ajsonhandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Ajsonhandler::BeginPlay()
{
	Super::BeginPlay();
    
    FetchBoxData();
}

// Called every frame
void Ajsonhandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Function to Start Fetching JSON Data
void Ajsonhandler::FetchBoxData()
{
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http) { UE_LOG(LogTemp, Error, TEXT("HTTP Module not found!")); return; }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &Ajsonhandler::OnBoxDataResponseReceived);
    Request->SetURL("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json");
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    if (!Request->ProcessRequest())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to start HTTP request!"));
    }
}

// Callback when HTTP Response is Received and parses the JSON data
void Ajsonhandler::OnBoxDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP request failed or response invalid!"));
        return;
    }

    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        FString JsonStr = Response->GetContentAsString();

        CachedBoxData.Empty();
        ParseJson(JsonStr, CachedBoxData);

        UE_LOG(LogTemp, Log, TEXT("Sending %d boxes to BoxHandler..."), CachedBoxData.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Error: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}

void Ajsonhandler::ParseJson(const FString& JsonStr, TArray<FBoxData>& ParsedBoxData)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON!"));
        return;
    }

    const TArray<TSharedPtr<FJsonValue>>* ObjectArray;
    if (!JsonObject->TryGetArrayField(TEXT("objects"), ObjectArray))
    {
        UE_LOG(LogTemp, Error, TEXT("JSON does not contain 'objects' array!"));
        return;
    }

    for (const TSharedPtr<FJsonValue>& ObjectValue : *ObjectArray)
    {
        TSharedPtr<FJsonObject> Object = ObjectValue->AsObject();
        if (!Object.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid object entry!"));
            continue;
        }

        FBoxData Data;

        if (Object->HasField(TEXT("type")))
        {
            Data.Type = Object->GetStringField(TEXT("type"));
        }

        if (Object->HasField(TEXT("transform")))
        {
            TSharedPtr<FJsonObject> TransformObject = Object->GetObjectField(TEXT("transform"));

            const TArray<TSharedPtr<FJsonValue>>* LocationArray;
            if (TransformObject->TryGetArrayField(TEXT("location"), LocationArray) && LocationArray->Num() == 3)
            {
                Data.Location = FVector(
                    (*LocationArray)[0]->AsNumber(),
                    (*LocationArray)[1]->AsNumber(),
                    (*LocationArray)[2]->AsNumber()
                );
            }

            const TArray<TSharedPtr<FJsonValue>>* RotationArray;
            if (TransformObject->TryGetArrayField(TEXT("rotation"), RotationArray) && RotationArray->Num() == 3)
            {
                Data.Rotation = FRotator(
                    (*RotationArray)[1]->AsNumber(),
                    (*RotationArray)[2]->AsNumber(),
                    (*RotationArray)[0]->AsNumber()
                );
            }

            const TArray<TSharedPtr<FJsonValue>>* ScaleArray;
            if (TransformObject->TryGetArrayField(TEXT("scale"), ScaleArray) && ScaleArray->Num() == 3)
            {
                Data.Scale = FVector(
                    (*ScaleArray)[0]->AsNumber(),
                    (*ScaleArray)[1]->AsNumber(),
                    (*ScaleArray)[2]->AsNumber()
                );
            }
        }

        if (Object->HasField(TEXT("health")))
        {
            Data.Health = Object->GetNumberField(TEXT("health"));
        }
        else
        {
            Data.Health = 10.f;
        }

        if (Object->HasField(TEXT("score")))
        {
            Data.Score = static_cast<int32>(Object->GetNumberField(TEXT("score")));
        }
        else
        {
            Data.Score = 100;
        }

        const TArray<TSharedPtr<FJsonValue>>* ColorArray;
        if (Object->TryGetArrayField(TEXT("color"), ColorArray) && ColorArray->Num() == 4)
        {
            Data.Color = FLinearColor(
                (*ColorArray)[0]->AsNumber(),
                (*ColorArray)[1]->AsNumber(),
                (*ColorArray)[2]->AsNumber(),
                (*ColorArray)[3]->AsNumber()
            );
        }
        else
        {
            Data.Color = FLinearColor::White;
        }

        ParsedBoxData.Add(Data);
    }

    UE_LOG(LogTemp, Log, TEXT("Parsed %d box objects from JSON"), ParsedBoxData.Num());
    //DisplayCachedBoxData();
}
void Ajsonhandler::DisplayCachedBoxData()
{
    UE_LOG(LogTemp, Log, TEXT("\n=== Displaying Cached Box Data ==="));

    for (const FBoxData& BoxData : CachedBoxData)
    {
        UE_LOG(LogTemp, Log, TEXT("--- Box Data ---"));
        UE_LOG(LogTemp, Log, TEXT("Type: %s"), *BoxData.Type);
        UE_LOG(LogTemp, Log, TEXT("Location: %s"), *BoxData.Location.ToString());
        UE_LOG(LogTemp, Log, TEXT("Rotation: %s"), *BoxData.Rotation.ToString());
        UE_LOG(LogTemp, Log, TEXT("Scale: %s"), *BoxData.Scale.ToString());
        UE_LOG(LogTemp, Log, TEXT("Health: %f"), BoxData.Health);
        UE_LOG(LogTemp, Log, TEXT("Score: %d"), BoxData.Score);
        UE_LOG(LogTemp, Log, TEXT("Color: R=%f G=%f B=%f A=%f"),
            BoxData.Color.R, BoxData.Color.G, BoxData.Color.B, BoxData.Color.A);
    }
}
