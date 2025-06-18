// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "jsonhandler/jsonhandler.h"

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

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // Dynamic - read objects
            if (JsonObject->HasTypedField<EJson::Array>(TEXT("objects")))
            {
                const TArray<TSharedPtr<FJsonValue>>& ObjectArray = JsonObject->GetArrayField(TEXT("objects"));

                for (const TSharedPtr<FJsonValue>& ObjectValue : ObjectArray)
                {
                    TSharedPtr<FJsonObject> ObjectData = ObjectValue->AsObject();
                    if (!ObjectData.IsValid())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Invalid object entry!"));
                        continue;
                    }

                    FString TypeName = ObjectData->GetStringField(TEXT("type"));

                    const TSharedPtr<FJsonObject> TransformObject = ObjectData->GetObjectField(TEXT("transform"));

                    FVector Location(0, 0, 0), Scale(1, 1, 1);
                    FRotator Rotation(0, 0, 0);

                    // Read transform arrays 
                    if (TransformObject->HasTypedField<EJson::Array>(TEXT("location")))
                    {
                        const auto LocationArray = TransformObject->GetArrayField(TEXT("location"));
                        Location.X = LocationArray[0]->AsNumber();
                        Location.Y = LocationArray[1]->AsNumber();
                        Location.Z = LocationArray[2]->AsNumber();
                    }

                    if (TransformObject->HasTypedField<EJson::Array>(TEXT("rotation")))
                    {
                        const auto RotationArray = TransformObject->GetArrayField(TEXT("rotation"));
                        Rotation.Roll = RotationArray[0]->AsNumber();
                        Rotation.Pitch = RotationArray[1]->AsNumber();
                        Rotation.Yaw = RotationArray[2]->AsNumber();
                    }

                    if (TransformObject->HasTypedField<EJson::Array>(TEXT("scale")))
                    {
                        const auto ScaleArray = TransformObject->GetArrayField(TEXT("scale"));
                        Scale.X = ScaleArray[0]->AsNumber();
                        Scale.Y = ScaleArray[1]->AsNumber();
                        Scale.Z = ScaleArray[2]->AsNumber();
                    }

                    UE_LOG(LogTemp, Log, TEXT("Object: Type=%s, Location=%s, Rotation=%s, Scale=%s"),
                        *TypeName, *Location.ToString(), *Rotation.ToString(), *Scale.ToString());

                    // Example: SpawnBox(TypeName, Location, Rotation, Scale);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No 'objects' array found in JSON."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Error: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}
