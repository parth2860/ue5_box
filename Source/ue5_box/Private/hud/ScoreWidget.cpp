// Fill out your copyright notice in the Description page of Project Settings.


#include "hud/ScoreWidget.h"

void UScoreWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Log if ScoreText is valid
    if (!ScoreText)
    {
        UE_LOG(LogTemp, Warning, TEXT("ScoreText is not valid in NativeConstruct! Check Blueprint binding."));
    }

    // Initialize the score
    CurrentScore = 0;

    // Update the score display
    UpdateScore(CurrentScore);
}

void UScoreWidget::UpdateScore(int32 NewScore)
{
    CurrentScore = NewScore;

    if (ScoreText)
    {
        ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), CurrentScore)));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ScoreText is null. Cannot update text."));
    }
}