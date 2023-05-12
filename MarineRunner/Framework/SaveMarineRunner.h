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
		float CurrentHealth;

	UPROPERTY(EditAnywhere)
		TMap <class AGun*, int32 > MagazineCapacityStorage;
	UPROPERTY(EditAnywhere)
		TMap <class AGun*, int32 > StoredAmmoStorage;

	UPROPERTY(EditAnywhere)
		TArray<class AGun*> Weapons;

	void SaveGame(int32 NewAmountOfFirstAidKits, float NewCurrentHealth,  TArray<class AGun*> WeaponsStorage);
	void LoadGame(class AMarineCharacter* MarinePawn);

	void ClearTMapValues();
};
