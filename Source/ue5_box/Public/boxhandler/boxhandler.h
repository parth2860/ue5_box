// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "boxhandler/data/boxdata.h"
#include "UObject/NoExportTypes.h"
#include "jsonhandler/jsonhandler.h"
#include "boxhandler.generated.h"

UCLASS()
class UE5_BOX_API Aboxhandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aboxhandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to spawn boxes from cached data
	void SpawnBoxesFromData(const TArray<FBoxData>& BoxDataArray);

	UPROPERTY()
	Ajsonhandler* JsonHandlerRef ;   
	//


};
