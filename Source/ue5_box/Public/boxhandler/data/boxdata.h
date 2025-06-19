// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoxData.generated.h" 

/**
 * 
 */

USTRUCT(BlueprintType)
struct FBoxData
{
    GENERATED_BODY()

    UPROPERTY()
    FString Type;

    UPROPERTY()
    FLinearColor Color;

    UPROPERTY()
    float Health;

    UPROPERTY()
    int32 Score;

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    UPROPERTY()
    FVector Scale;
};

class UE5_BOX_API boxdata
{
public:
	boxdata();
	~boxdata();
};
