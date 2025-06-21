// Copyright Epic Games, Inc. All Rights Reserved.

#include "ue5_boxCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "boxhandler/boxhandler.h"
#include "jsonhandler/json_boxspawner.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Aue5_boxCharacter

Aue5_boxCharacter::Aue5_boxCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	weponmesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("weponmesh"));
	weponmesh->SetupAttachment(FirstPersonCameraComponent);
	//weponmesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	weponmesh->SetCollisionProfileName(FName("NoCollision"));

	muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("muzzle"));
	muzzle->SetupAttachment(weponmesh);

}

void Aue5_boxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &Aue5_boxCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &Aue5_boxCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Aue5_boxCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Aue5_boxCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &Aue5_boxCharacter::LookInput);

		//lmbaction
		EnhancedInputComponent->BindAction(LMB_Action, ETriggerEvent::Triggered, this, &Aue5_boxCharacter::lmb_action);
		//spawnedaction
		EnhancedInputComponent->BindAction(Spawn_Action, ETriggerEvent::Started, this, &Aue5_boxCharacter::spawned_action);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void Aue5_boxCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void Aue5_boxCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void Aue5_boxCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void Aue5_boxCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void Aue5_boxCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void Aue5_boxCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}
void Aue5_boxCharacter::lmb_action()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("lmb Action Triggered!"));
	FireLineTrace();
}

void Aue5_boxCharacter::spawned_action()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Spawned Action Triggered!"));

	Ajson_boxspawner* data = Cast<Ajson_boxspawner>(UGameplayStatics::GetActorOfClass(GetWorld(), Ajson_boxspawner::StaticClass()));

	data->ParseAndSpawnBoxes();
}
void Aue5_boxCharacter::FireLineTrace()
{
	// Start location from weapon mesh
	FVector MuzzleLocation = muzzle->GetComponentLocation();

	// Adjust forward direction offset (example: + forward * 50 units)
	FVector ForwardVector = muzzle->GetForwardVector();
	FVector StartLocation = MuzzleLocation + (ForwardVector * 50.0f); // adjust 50 as needed

	FVector EndLocation = StartLocation + (ForwardVector * 10000.0f); // 10,000 units trace

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // ignore self

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);


	// Debug line to visualize trace
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();

		// Draw impact point — small green sphere
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);

		if (HitActor)
		{
			// Print hit actor name and impact location
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
				FString::Printf(TEXT("Hit Actor: %s at Location: %s"),
					*HitActor->GetName(),
					*HitResult.ImpactPoint.ToString()));

			// Specific hit condition — if hit Aboxhandler class
			if (HitActor->IsA(Aboxhandler::StaticClass()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Hit Aboxhandler — Score!"));
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, TEXT("No Hit"));
	}
}
