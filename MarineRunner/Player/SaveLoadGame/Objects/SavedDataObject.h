// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"
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

	void AddCustomSaveData(const int32& SavedCustomDataKey, const FCustomDataSaved& SavedCustomData);
	void RemoveCustomSaveData(const int32& SavedCustomDataID);
	int32 CreateUniqueIDForObject() const;

	FORCEINLINE const TMap<int32, FCustomDataSaved>& GetCustomSavedData() const { return CustomSavedData; }
	FORCEINLINE void SetCustomSavedData(TMap<int32, FCustomDataSaved>& NewSavedData) { CustomSavedData = NewSavedData; }

	void LoadObjectsData();
	void StartLoadingObjects();
	void UpdateObjectsData();
	void RestartObjectsData();
	// remove elements from TempCustomSavedData with bValueSaveFromThebeginning == false
	void RemoveTempCustomSavedData();
private:
	// All objects that player interacted with SaveCustomDataInterface are stored here, after loading a game 
	// takes all elements and loads data in given actor or spawns actor with saved data
	UPROPERTY(EditAnywhere, Category = "Saved Data Settings")
	TMap<int32, FCustomDataSaved> CustomSavedData;
	// The same as CustomSavedData except that if player dies before saving a game then takes elements from list and calls RestartData on elements in list and deletes from CustomSavedData
	// its useful for restarting a game e.g player opened the chest but died after so it has to restart the chest
	UPROPERTY(EditAnywhere, Category = "Saved Data Settings")
	TMap<int32, FCustomDataSaved> TempCustomSavedData;
	UPROPERTY(EditAnywhere, Category = "Saved Data Settings")
	FFloatRange RandomDataIDRangeForObject = FFloatRange(1.f, 1000.f);
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
	float StartLoadObjectsDataTime = 0.05f;

};
