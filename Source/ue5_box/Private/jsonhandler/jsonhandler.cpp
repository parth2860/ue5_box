// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
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
    SendHttpRequest();
}

// Called every frame
void Ajsonhandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//----------------------------------------------------------------------------------------------------------
void Ajsonhandler::SendHttpRequest()
{
    // Create HTTP request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json")); // Replace with your URL
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Bind the response callback
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &Ajsonhandler::OnHttpResponseReceived);

    // Process the request
    if (!HttpRequest->ProcessRequest())
    {
        LogError(TEXT("Failed to process HTTP request."));
    }
}

void Ajsonhandler::OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        LogError(TEXT("HTTP request failed or response is invalid."));
        return;
    }

    if (Response->GetResponseCode() != 200)
    {
        FString ErrorMessage = FString::Printf(TEXT("HTTP Error: %d"), Response->GetResponseCode());
        LogError(ErrorMessage);
        return;
    }

    const FString ResponseString = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);


    // Print the raw JSON response string
    PrintJsonResponse(ResponseString);



    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        LogError(TEXT("Failed to parse JSON response."));
        return;
    }

    // ProcessJson(JsonObject);
    ParsedJsonData.Empty(); // Clear existing data
    ParsedJsonData.Add(JsonObject);
    //
    // Print the raw JSON response string
   // UE_LOG(LogTemp, Warning, TEXT("Raw JSON Response: %s"), *ResponseString);
}
TArray<TSharedPtr<FJsonObject>> Ajsonhandler::GetParsedJsonData() const
{

    // Perform your JSON parsing logic here
    if (ParsedJsonData.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Parsing JSON data..."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ParsedJsonData array is empty."));
    }
    return ParsedJsonData;
}
void Ajsonhandler::PrintJsonResponse(const FString& JsonResponse)
{
    // Log the raw JSON string received
   // UE_LOG(LogTemp, Warning, TEXT("Raw JSON Response: %s"), *JsonResponse);
}
void Ajsonhandler::ProcessJson(const TSharedPtr<FJsonObject>& JsonObject)
{
    // Loop through all fields in the root JSON object dynamically
    for (const auto& Pair : JsonObject->Values)
    {
        const FString& Key = Pair.Key;
        const TSharedPtr<FJsonValue>& JsonValue = Pair.Value;

        // Process each field dynamically
        //UE_LOG(LogTemp, Log, TEXT("Key: %s"), *Key);
        ProcessJsonValue(JsonValue);
    }
}
//Check all field in json Object
void Ajsonhandler::ProcessJsonValue(const TSharedPtr<FJsonValue>& JsonValue)
{
    switch (JsonValue->Type)
    {
    case EJson::Null:
        UE_LOG(LogTemp, Log, TEXT("Value is Null"));
        break;

    case EJson::String:
        //UE_LOG(LogTemp, Log, TEXT("Value (String): %s"), *JsonValue->AsString());
        break;

    case EJson::Number:
        //UE_LOG(LogTemp, Log, TEXT("Value (Number): %f"), JsonValue->AsNumber());
        break;

    case EJson::Boolean:
        // UE_LOG(LogTemp, Log, TEXT("Value (Boolean): %s"), JsonValue->AsBool() ? TEXT("true") : TEXT("false"));
        break;

    case EJson::Array:
    {
        // Handle array of values
        const TArray<TSharedPtr<FJsonValue>>& JsonArray = JsonValue->AsArray();
        for (const TSharedPtr<FJsonValue>& ArrayItem : JsonArray)
        {
            //UE_LOG(LogTemp, Log, TEXT("Array Item:"));
            ProcessJsonValue(ArrayItem); // Recursively process array items
        }
        break;
    }

    case EJson::Object:
    {
        // Handle nested object
        TSharedPtr<FJsonObject> NestedObject = JsonValue->AsObject();
        for (const auto& Pair : NestedObject->Values)
        {
            //UE_LOG(LogTemp, Log, TEXT("Nested Object Key: %s"), *Pair.Key);
            ProcessJsonValue(Pair.Value); // Recursively process nested object fields
        }
        break;
    }

    default:
        UE_LOG(LogTemp, Log, TEXT("Unhandled JSON Type"));
        break;
    }
}

void Ajsonhandler::LogError(const FString& ErrorMessage)
{
    UE_LOG(LogTemp, Error, TEXT("JSON Handler Error: %s"), *ErrorMessage);
}