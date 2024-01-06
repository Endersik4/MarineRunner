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
		TMap <class AGun*, int32 > MagazineCapacityStorage;

	UPROPERTY(EditAnywhere)
		TArray<class AGun*> WeaponsSaved;

	UPROPERTY(EditAnywhere)
		TMap < int32, class AGun* > WeaponsStorageSaved;

	UPROPERTY(EditAnywhere)
		class AGun* MarineGun;

	UPROPERTY(EditAnywhere)
		FVector SavedPlayerLocation;
	UPROPERTY(EditAnywhere)
		FRotator SavedPlayerRotation;

	UPROPERTY(EditAnywhere)
		TMap<FString, FItemStruct> Inventory_ItemsSaved;

	UPROPERTY(EditAnywhere)
		int32 SaveNumber = 0;

	UPROPERTY(EditAnywhere)
		float TotalPlayTimeInSeconds = 0.f;

	UPROPERTY(EditAnywhere)
		AActor* CurrentCheckpoint;

	UPROPERTY()
		TArray<FCustomDataSaved> CustomSavedData;

	void PrepareSaveGame(const FString& NewSaveName = "ManualSave_", bool bAddSaveNumber = true);
	FString OriginalSaveName;
	bool bAddSaveNumber;

	void SaveGame(float CurrentHealth, class AGun* CurrentMarineGun, TMap < int32, class AGun* > CurrentWeaponsStorage, TMap<FString, FItemStruct> CurrentInventory_ItemsSaved);
	void CopySaveInfoToCurrentGameInstance(UWorld* CurrentWorld);
	void SaveOtherObjectsData(class ASavedDataObject* OtherObjectsData);

	void MakeJsonFileWithSaveInfo(APlayerController* PlayerController, const FString & CurrentLevelName);

	void LoadGame(class AMarineCharacter* MarinePawn, class UMarineRunnerGameInstance* GameInstance);
	void LoadOtherObjectsData(class ASavedDataObject* OtherObjectsData);

	FString GetSaveGameName();

private:
	void LoadEquipedGuns(class AMarineCharacter* MarinePawn);

	// return Screnshot path
	FString TakeSaveScreenshot(APlayerController* PlayerController);
	FString ConvertCurrentDateToText();

};
