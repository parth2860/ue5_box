// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "jsonhandler/jsonhandler.h"
#include "boxhandler/boxhandler.h"
#include "jsonhandler/json_boxspawner.h"

// Sets default values
Ajson_boxspawner::Ajson_boxspawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Ajson_boxspawner::BeginPlay()
{
	Super::BeginPlay();
	
    ParseAndSpawnBoxes();

}

// Called every frame
void Ajson_boxspawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void Ajson_boxspawner::ParseAndSpawnBoxes()
{
    //-----------------------------------------------------------------------------------------------------------------
    // Search for Acp_data class in the world
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), Ajsonhandler::StaticClass(), FoundActors);//

    if (FoundActors.Num() > 0)
    {
        DataClass = Cast<Ajsonhandler>(FoundActors[0]);
        if (DataClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Acp_data found and initialized successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast Acp_data."));
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Acp_data actor found in the world."));
        return;
    }

    //-----------------------------------------------------------------------------------------------------------------
    // Ensure the DataClass is valid and fetch JSON data
    if (DataClass)
    {
        // Assuming GetParsedJsonData returns the full array of objects 
        JsonObjectsData = DataClass->GetParsedJsonData();

        if (JsonObjectsData.Num() > 0)
        {
            // Get the root object from the JSON data
            TSharedPtr<FJsonObject> RootObject = JsonObjectsData[0]; // Assuming root object is the first element in the array

            // Get the 'types' array from the JSON root
            if (RootObject.IsValid() && RootObject->HasField(TEXT("types")))
            {
                TArray<TSharedPtr<FJsonValue>> TypesArray = RootObject->GetArrayField(TEXT("types"));

                // Iterate through the objects array 
                if (RootObject->HasField(TEXT("objects")))
                {
                    TArray<TSharedPtr<FJsonValue>> ObjectsArray = RootObject->GetArrayField(TEXT("objects"));

                    for (const TSharedPtr<FJsonValue>& ObjectValue : ObjectsArray)
                    {
                        TSharedPtr<FJsonObject> JsonObject = ObjectValue->AsObject();
                        if (JsonObject.IsValid())
                        {
                            // Ensure the 'type' field exists and is valid
                            if (!JsonObject->HasField(TEXT("type")) || JsonObject->GetStringField(TEXT("type")).IsEmpty())
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Field 'type' not found or is empty in the object."));
                                continue; // Skip if 'type' is missing or empty
                            }

                            FString ObjectType = JsonObject->GetStringField(TEXT("type"));
                            UE_LOG(LogTemp, Warning, TEXT("Parsing object of type: %s"), *ObjectType);

                            // Find the corresponding type data by matching the object type
                            TSharedPtr<FJsonObject> TypeData = nullptr;
                            for (const TSharedPtr<FJsonValue>& TypeValue : TypesArray)
                            {
                                TSharedPtr<FJsonObject> TypeObject = TypeValue->AsObject();
                                if (TypeObject.IsValid() && TypeObject->GetStringField(TEXT("name")) == ObjectType)
                                {
                                    TypeData = TypeObject;
                                    break;
                                }
                            }

                            if (!TypeData.IsValid())
                            {
                                UE_LOG(LogTemp, Error, TEXT("Failed to find type data for type: %s"), *ObjectType);
                                continue; // Skip if we cannot find type data for this object type
                            }

                            // Parse transform data (location, rotation, scale)//------------------------------
                            if (!JsonObject->HasField(TEXT("transform")))
                            {
                                UE_LOG(LogTemp, Error, TEXT("Transform data not found for object type: %s"), *ObjectType);
                                continue; // Skip if transform data is missing
                            }

                            TSharedPtr<FJsonObject> TransformObject = JsonObject->GetObjectField(TEXT("transform"));

                            // Make sure location, rotation, and scale exist
                            if (!TransformObject->HasField(TEXT("location")) || !TransformObject->HasField(TEXT("rotation")) || !TransformObject->HasField(TEXT("scale")))
                            {
                                UE_LOG(LogTemp, Error, TEXT("Missing location, rotation, or scale data in transform for object type: %s"), *ObjectType);
                                continue; // Skip if any transform data is missing
                            }

                            // Parse location, rotation, and scale//------------------------------
                            TArray<TSharedPtr<FJsonValue>> LocationArray = TransformObject->GetArrayField(TEXT("location"));
                            FVector Location(
                                LocationArray[0]->AsNumber(),
                                LocationArray[1]->AsNumber(),
                                LocationArray[2]->AsNumber()
                            );

                            TArray<TSharedPtr<FJsonValue>> RotationArray = TransformObject->GetArrayField(TEXT("rotation"));
                            FRotator Rotation(
                                RotationArray[0]->AsNumber(),
                                RotationArray[1]->AsNumber(),
                                RotationArray[2]->AsNumber()
                            );

                            TArray<TSharedPtr<FJsonValue>> ScaleArray = TransformObject->GetArrayField(TEXT("scale"));
                            FVector Scale(
                                ScaleArray[0]->AsNumber(),
                                ScaleArray[1]->AsNumber(),
                                ScaleArray[2]->AsNumber()
                            );

                            // Parse type-specific data like color, health, and score//------------------------------
                            TArray<TSharedPtr<FJsonValue>> ColorArray = TypeData->GetArrayField(TEXT("color"));
                            FLinearColor Color = FLinearColor::FromSRGBColor(FColor(
                                ColorArray[0]->AsNumber(),
                                ColorArray[1]->AsNumber(),
                                ColorArray[2]->AsNumber()
                            ));

                            int32 Health = TypeData->GetNumberField(TEXT("health"));
                            int32 Score = TypeData->GetNumberField(TEXT("score"));

                            // Spawn the cube with all parsed data
                            SpawnCube(ObjectType, Location, Rotation, Scale, Color, Health, Score);
                        }
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("No 'objects' field found in the root JSON object."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("No 'types' field found in the root JSON object."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("No JSON objects found in the data array. Check GetParsedJsonData() implementation."));
        }
    }
}



// Helper function to get type data from types array
TSharedPtr<FJsonObject> Ajson_boxspawner::GetTypeDataByName(const TArray<TSharedPtr<FJsonValue>>& TypesArray, const FString& TypeName)
{
    for (const TSharedPtr<FJsonValue>& TypeValue : TypesArray)
    {
        TSharedPtr<FJsonObject> Type = TypeValue->AsObject();
        if (Type->GetStringField(TEXT("name")) == TypeName)
        {
            return Type;
        }
    }
    return nullptr;
}

// Function to spawn the cube actor
void Ajson_boxspawner::SpawnCube(FString Type, FVector Location, FRotator Rotation, FVector Scale, FLinearColor Color, int32 Health, int32 Score)
{
    if (UWorld* World = GetWorld())
    {
        // Spawn the cube actor
        FActorSpawnParameters SpawnParams;
        Aboxhandler* Cube = World->SpawnActor<Aboxhandler>(Aboxhandler::StaticClass(), Location, Rotation, SpawnParams);

        // Initialize the cube with data
        if (Cube)
        {
            Cube->InitializeBox(Type, Location, Rotation, Scale, Color, Health, Score);
            UE_LOG(LogTemp, Warning, TEXT("Cube of type '%s' spawned successfully."), *Type);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn cube of type '%s'."), *Type);
        }
    }
}
