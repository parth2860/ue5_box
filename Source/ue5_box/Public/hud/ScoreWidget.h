// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_BOX_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))//to used in cpp
		UTextBlock* ScoreText;

	// Variable to store the score value
	int32 CurrentScore;

	// The function to update the score
	UFUNCTION(Category = "Score")
	void UpdateScore(int32 NewScore);

protected:

	// Native construct to initialize widget
	virtual void NativeConstruct() override;
};
