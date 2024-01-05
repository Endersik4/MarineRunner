// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"
#include "SavedDataObject.generated.h"

/// <summary>
/// Object that saves other object data (for example chest or door was open - save it). Should be only one in the level.
/// </summary>
UCLASS()
class MARINERUNNER_API ASavedDataObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASavedDataObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	void AddCustomSaveData(FCustomDataSaved & NewCustomSavedData);
	void AddCustomSaveData(ISaveCustomDataInterface * ObjectToSave, int32 StateOfObjectToSave);

	FORCEINLINE TArray<FCustomDataSaved> GetCustomSavedData() const { return CustomSavedData; }

	FORCEINLINE void SetCustomSavedData(TArray<FCustomDataSaved>& NewSavedData) { CustomSavedData = NewSavedData; }

	void LoadObjectsData();

private:
	TArray<FCustomDataSaved> CustomSavedData;
};
