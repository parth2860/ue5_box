// Copyright Epic Games, Inc. All Rights Reserved.


#include "ue5_boxPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "ue5_boxCameraManager.h"

Aue5_boxPlayerController::Aue5_boxPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = Aue5_boxCameraManager::StaticClass();
}

void Aue5_boxPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
