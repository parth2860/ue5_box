// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "jsonhandler.generated.h"

UCLASS()
class UE5_BOX_API Ajsonhandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Ajsonhandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
    //----------------------------------------------------------------------------------------------------------

    // HTTP Request Handler
    void SendHttpRequest();

    // Response Handlers
    void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    // Error Logger
    void LogError(const FString& ErrorMessage);

    // Print JSON raw response
    void PrintJsonResponse(const FString& JsonResponse);

    // Process JSON dynamically
    void ProcessJson(const TSharedPtr<FJsonObject>& JsonObject);

    // Helper function to process nested JSON objects and arrays dynamically
    void ProcessJsonValue(const TSharedPtr<FJsonValue>& JsonValue);

    //----------------------------------------------------------------------------------------------------------
    // Function to retrieve the parsed JSON objects
    TArray<TSharedPtr<FJsonObject>> GetParsedJsonData() const;


    // Array to hold the parsed JSON data
    TArray<TSharedPtr<FJsonObject>> ParsedJsonData;
};
