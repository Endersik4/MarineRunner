// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"
#include "SaveMarineRunner.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API USaveMarineRunner : public USaveGame
{
	GENERATED_BODY()


public:
	USaveMarineRunner();

	UPROPERTY(EditAnywhere)
	float CurrentHealthSaved = 0.f;
	UPROPERTY(EditAnywhere)
	bool bShowHealBar = false;
	UPROPERTY(EditAnywhere)
	bool bShowDashBar = false;
	UPROPERTY(EditAnywhere)
	bool bShowSlowMotionBar = false;

	UPROPERTY(EditAnywhere) //ztgk, delete later
	int32 CheckpointNumber = 0;

	UPROPERTY(EditAnywhere)
	FVector SavedPlayerLocation = FVector::Zero();
	UPROPERTY(EditAnywhere)
	FRotator SavedPlayerRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	TArray<FItemStruct> Inventory_ItemsSaved;
	UPROPERTY(EditAnywhere)
	TArray<FItemStruct> Inventory_RecipesSaved;
	UPROPERTY(EditAnywhere)
	TMap<int32, FSoftClassPath> WeaponInventory_Saved;

	UPROPERTY(EditAnywhere)
	int32 SaveNumber = 0;
	UPROPERTY(EditAnywhere)
	float TotalPlayTimeInSeconds = 0.f;
	UPROPERTY(EditAnywhere)
	float LastGameTimePlayTime = 0.f;

	UPROPERTY(EditAnywhere)
	TMap<int32, FCustomDataSaved> SavedCustomData;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> SavedExplorationMusic = nullptr;

	void PrepareSaveGame(const FString& NewSaveName = "ManualSave_", bool bAddSaveNumber = true);

	UPROPERTY(Transient)
	FString OriginalSaveName = FString();
	UPROPERTY(Transient)
	bool bAddSaveNumber = false;

	void TransferSavedVariablesToGameInstance(TObjectPtr<UWorld> CurrentWorld, const FString& _WildCard);

	void MakeJsonFileWithSaveInfo(TObjectPtr<APlayerController> PlayerController, const FString& CurrentLevelName);

	void LoadGame(TObjectPtr<class AMarineCharacter> MarinePawn, TObjectPtr<class UMarineRunnerGameInstance> GameInstance);
	void LoadOtherObjectsData(TObjectPtr<class ASavedDataObject> OtherObjectsData);

	FString GetSaveGameName();

private:

	// return Screnshot path
	FString TakeSaveScreenshot(TObjectPtr<APlayerController> PlayerController);
	FString ConvertCurrentDateToText();

};
