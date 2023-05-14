// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
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
		int32 AmountOfFirstAidKits;

	UPROPERTY(EditAnywhere)
		float CurrentHealthSaved;

	UPROPERTY(EditAnywhere)
		TMap <class AGun*, int32 > MagazineCapacityStorage;
	UPROPERTY(EditAnywhere)
		TMap <class AGun*, int32 > StoredAmmoStorage;

	UPROPERTY(EditAnywhere)
		TArray<class AGun*> WeaponsSaved;

	UPROPERTY(EditAnywhere)
		TMap < int32, class AGun* > WeaponsStorageSaved;

	UPROPERTY(EditAnywhere)
		class AGun* MarineGun;

	UPROPERTY(EditAnywhere)
		FVector CheckpointLocation;

	void SaveGame(int32 CurrentAmountOfFirstAidKits, float CurrentHealth, class AGun* CurrentMarineGun, TMap < int32, class AGun* > CurrentWeaponsStorage);
	void LoadGame(class AMarineCharacter* MarinePawn);

	void ClearTMapValues();
};
