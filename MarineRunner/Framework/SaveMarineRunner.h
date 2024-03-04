// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MarineRunner/Inventory/InventoryComponent.h"

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
		float CurrentHealthSaved;
	UPROPERTY(EditAnywhere)
		bool bShowHealBar;
	UPROPERTY(EditAnywhere)
		bool bShowDashBar;
	UPROPERTY(EditAnywhere)
		bool bShowSlowMotionBar;

	UPROPERTY(EditAnywhere)
		FVector SavedPlayerLocation;
	UPROPERTY(EditAnywhere)
		FRotator SavedPlayerRotation;

	UPROPERTY(EditAnywhere)
		TArray<FItemStruct> Inventory_ItemsSaved;
	UPROPERTY(EditAnywhere)
		TArray<FItemStruct> Inventory_RecipesSaved;
	UPROPERTY(EditAnywhere)
		TMap<int32, FString> WeaponInventory_Saved;

	UPROPERTY(EditAnywhere)
		int32 SaveNumber = 0;
	UPROPERTY(EditAnywhere)
		float TotalPlayTimeInSeconds = 0.f;
	UPROPERTY(EditAnywhere)
		float LastGameTimePlayTime = 0.f;

	UPROPERTY(EditAnywhere)
		TMap<int32, FCustomDataSaved> SavedCustomData;

	UPROPERTY(EditAnywhere)
		TObjectPtr<USoundBase> SavedExplorationMusic;

	void PrepareSaveGame(const FString& NewSaveName = "ManualSave_", bool bAddSaveNumber = true);
	FString OriginalSaveName;
	bool bAddSaveNumber;

	void TransferSavedVariablesToGameInstance(TObjectPtr<UWorld> CurrentWorld, const FString& _WildCard);

	void MakeJsonFileWithSaveInfo(TObjectPtr<APlayerController> PlayerController, const FString & CurrentLevelName);

	void LoadGame(TObjectPtr<class AMarineCharacter> MarinePawn, TObjectPtr<class UMarineRunnerGameInstance> GameInstance);
	void LoadOtherObjectsData(TObjectPtr<class ASavedDataObject> OtherObjectsData);

	FString GetSaveGameName();

private:

	// return Screnshot path
	FString TakeSaveScreenshot(TObjectPtr<APlayerController> PlayerController);
	FString ConvertCurrentDateToText();

};
