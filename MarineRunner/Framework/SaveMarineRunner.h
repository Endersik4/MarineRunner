// Fill out your copyright notice in the Description page of Project Settings.

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
		TMap <class AGun*, int32 > MagazineCapacityStorage;

	UPROPERTY(EditAnywhere)
		TArray<class AGun*> WeaponsSaved;

	UPROPERTY(EditAnywhere)
		TMap < int32, class AGun* > WeaponsStorageSaved;

	UPROPERTY(EditAnywhere)
		class AGun* MarineGun;

	UPROPERTY(EditAnywhere)
		FVector CheckpointLocation;

	UPROPERTY(EditAnywhere)
		TMap<FString, FItemStruct> Inventory_ItemsSaved;

	void SaveGame(float CurrentHealth, class AGun* CurrentMarineGun, TMap < int32, class AGun* > CurrentWeaponsStorage, TMap<FString, FItemStruct> CurrentInventory_ItemsSaved);
	void LoadGame(class AMarineCharacter* MarinePawn);

	void ClearTMapValues();

private:
	void LoadEquipedGuns(class AMarineCharacter* MarinePawn);
};
