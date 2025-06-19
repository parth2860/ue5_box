// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "boxhandler/data/boxdata.h"
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
	// Function to start fetching box data
	UFUNCTION(BlueprintCallable, Category = "Box Data")
	void FetchBoxData();

	// HTTP response callback
	void OnBoxDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//void ParseJson(const FString& JsonString, TArray<FBoxData>& OutBoxDataArray);
	void ParseJson(const FString& JsonStr, TArray<FBoxData>& ParsedBoxData);// parse & fill data

	// Store parsed data here
	TArray<FBoxData> CachedBoxData;//store data

	// Provide the data to anyone asking------------------------
	const TArray<FBoxData>& GetParsedBoxData() const { return CachedBoxData; }//read data later
	
	void DisplayCachedBoxData();
};
