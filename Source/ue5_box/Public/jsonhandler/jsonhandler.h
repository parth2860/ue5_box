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
	// Function to start fetching box data
	UFUNCTION(BlueprintCallable, Category = "Box Data")
	void FetchBoxData();

	// HTTP response callback
	void OnBoxDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// void CreateBox(const FString& Type, const FString& Color, float Health, int32 Score);
};
