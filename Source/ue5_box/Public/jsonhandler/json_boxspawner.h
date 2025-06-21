// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "json_boxspawner.generated.h"

UCLASS()
class UE5_BOX_API Ajson_boxspawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Ajson_boxspawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to fetch JSON data from the Acp_data class and spawn boxes
	void ParseAndSpawnBoxes();

	// Helper function to get type data by name
	TSharedPtr<FJsonObject> GetTypeDataByName(const TArray<TSharedPtr<FJsonValue>>& TypesArray, const FString& TypeName);

	// Function to spawn the cube actor
	void SpawnCube(FString Type, FVector Location, FRotator Rotation, FVector Scale, FLinearColor Color, int32 Health, int32 Score);

	// Reference to the Data Class that holds the JSON data
	class Ajsonhandler* DataClass;

	// Store parsed JSON objects for use
	TArray<TSharedPtr<FJsonObject>> JsonObjectsData;
};
