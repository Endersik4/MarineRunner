// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "SaveMarineRunner.generated.h"

class ISaveCustomDataInterface;

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
		FVector SavedPlayerLocation;
	UPROPERTY(EditAnywhere)
		FRotator SavedPlayerRotation;

	UPROPERTY(EditAnywhere)
		TArray<FItemStruct> Inventory_ItemsSaved;
	UPROPERTY(EditAnywhere)
		TArray<FItemStruct> Inventory_RecipesSaved;
	UPROPERTY(EditAnywhere)
		TMap<int32, TSubclassOf<class AGun>> WeaponInventory_Saved;

	UPROPERTY(EditAnywhere)
		int32 SaveNumber = 0;
	UPROPERTY(EditAnywhere)
		float TotalPlayTimeInSeconds = 0.f;

	UPROPERTY(EditAnywhere)
		TMap<int32, FCustomDataSaved> SavedCustomData;

	UPROPERTY(EditAnywhere)
		USoundBase* CurrentExplorationMusic;

	void PrepareSaveGame(const FString& NewSaveName = "ManualSave_", bool bAddSaveNumber = true);
	FString OriginalSaveName;
	bool bAddSaveNumber;

	void CopySaveInfoToCurrentGameInstance(UWorld* CurrentWorld);

	void MakeJsonFileWithSaveInfo(APlayerController* PlayerController, const FString & CurrentLevelName);

	void LoadGame(class AMarineCharacter* MarinePawn, class UMarineRunnerGameInstance* GameInstance);
	void LoadOtherObjectsData(class ASavedDataObject* OtherObjectsData);

	FString GetSaveGameName();

private:

	// return Screnshot path
	FString TakeSaveScreenshot(APlayerController* PlayerController);
	FString ConvertCurrentDateToText();

};
