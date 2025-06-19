// Fill out your copyright notice in the Description page of Project Settings.


#include "boxhandler/boxhandler.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
Aboxhandler::Aboxhandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    JsonHandlerRef = nullptr;  // important — init null
}

void Aboxhandler::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void Aboxhandler::BeginPlay()
{
    Super::BeginPlay();
    //
    // ✅ Auto-find JsonHandler actor placed in level
    TArray<AActor*> FoundHandlers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), Ajsonhandler::StaticClass(), FoundHandlers);

    if (FoundHandlers.Num() > 0)
    {
        JsonHandlerRef = Cast<Ajsonhandler>(FoundHandlers[0]);
        UE_LOG(LogTemp, Log, TEXT("JsonHandlerRef found successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("JsonHandler not found in level!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("BoxHandler BeginPlay - will call SpawnBoxesFromData after 5 seconds"));
    //
    // 🔁 Delay to give HTTP time to complete
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            UE_LOG(LogTemp, Log, TEXT("BoxHandler - Timer triggered - calling SpawnBoxesFromData"));

            if (JsonHandlerRef)
            {
                const TArray<FBoxData>& FetchedData = JsonHandlerRef->GetParsedBoxData();
                SpawnBoxesFromData(FetchedData);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("JsonHandlerRef is NULL! Can't fetch data. Will retry in 2 seconds."));
            }

        }, 5.0f, false);
}

void Aboxhandler::SpawnBoxesFromData(const TArray<FBoxData>& BoxDataArray)
{
    UE_LOG(LogTemp, Log, TEXT("BoxHandler: Received %d boxes."), BoxDataArray.Num());

    for (const FBoxData& BoxData : BoxDataArray)
    {
        UE_LOG(LogTemp, Log, TEXT("--- Box Data ---"));
        UE_LOG(LogTemp, Log, TEXT("Type: %s"), *BoxData.Type);
        UE_LOG(LogTemp, Log, TEXT("Location: %s"), *BoxData.Location.ToString());
        UE_LOG(LogTemp, Log, TEXT("Rotation: %s"), *BoxData.Rotation.ToString());
        UE_LOG(LogTemp, Log, TEXT("Scale: %s"), *BoxData.Scale.ToString());
        UE_LOG(LogTemp, Log, TEXT("Health: %f"), BoxData.Health);
        UE_LOG(LogTemp, Log, TEXT("Score: %d"), BoxData.Score);
        UE_LOG(LogTemp, Log, TEXT("Color: R=%f G=%f B=%f A=%f"),
            BoxData.Color.R, BoxData.Color.G, BoxData.Color.B, BoxData.Color.A);
    }
}