// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

    //--------------------------------------------------------------

    // Function to initialize the box with the required properties
    void InitializeBox(FString Boxtype, FVector Location, FRotator Rotation, FVector Scale, FLinearColor Color, int32 Health, int32 Score);

    // Function to apply damage when the box is shot
    void ApplyDamage();

    // Function to handle the destruction of the box
    void DestroyBox();

    // Function to update the score
    void UpdateScore();

    // Health and score variables
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CubeHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CubeScore;

    // Member variables to hold cube data
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString CubeType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector CubeLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FRotator CubeRotation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector CubeScale;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FLinearColor CubeColor;

    // Mesh for the box
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BoxMesh;

    // Material for the box
    UPROPERTY(VisibleAnywhere)
    UMaterialInstanceDynamic* BoxMaterial;

    // Display total score
   // static int32 TotalScore;
    int32 TotalScore;


};
