// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

USaveMarineRunner::USaveMarineRunner()
{
	AmountOfFirstAidKits = 0;
	CurrentHealthSaved = 0;
	ClearTMapValues();
}

void USaveMarineRunner::SaveGame(int32 CurrentAmountOfFirstAidKits, float CurrentHealth, AGun* CurrentMarineGun, TMap < int32, AGun* > CurrentWeaponsStorage)
{
	ClearTMapValues();
	AmountOfFirstAidKits = CurrentAmountOfFirstAidKits;
	CurrentHealthSaved = CurrentHealth;

	CurrentWeaponsStorage.GenerateValueArray(WeaponsSaved);
	WeaponsStorageSaved = CurrentWeaponsStorage;
	MarineGun = CurrentMarineGun;
	for (auto& Storage : WeaponsSaved)
	{
		MagazineCapacityStorage.Add(Storage, Storage->GetMagazineCapacity());
		StoredAmmoStorage.Add(Storage, Storage->GetStoredAmmo());
	}
}

void USaveMarineRunner::LoadGame(class AMarineCharacter* MarinePawn)
{
	if (MarinePawn == nullptr) return;

	MarinePawn->SetFirstAidKits(AmountOfFirstAidKits);
	MarinePawn->SetHealth(CurrentHealthSaved);
	MarinePawn->SetQuickSelect(WeaponsStorageSaved);

	if (!MarineGun) return;

	MarinePawn->SetGun(MarineGun);

	for (auto& CurrentWeapon : WeaponsSaved)
	{
		if (MagazineCapacityStorage.Find(CurrentWeapon))
		{
			CurrentWeapon->SetMagazineCapacity(*MagazineCapacityStorage.Find(CurrentWeapon));
		}
		if (StoredAmmoStorage.Find(CurrentWeapon))
		{
			CurrentWeapon->SetStoredAmmo(*StoredAmmoStorage.Find(CurrentWeapon));
		}

		if (MarinePawn->GetGun() == CurrentWeapon)
		{	
			CurrentWeapon->EquipWeapon(MarinePawn, false);
			CurrentWeapon->SetGunSwayWhileMovingTimer();
		}
		else
		{
			CurrentWeapon->EquipWeapon(MarinePawn, false, false);
			CurrentWeapon->SetGunSwayWhileMovingTimer(true);
			CurrentWeapon->SetActorHiddenInGame(true);
		}
		CurrentWeapon->SetCanGunSwayTick(true);
		CurrentWeapon->SetActorRelativeLocation(MarineGun->GetRelativeLocationInPawn());	
	}
}

void USaveMarineRunner::ClearTMapValues()
{
	WeaponsSaved.Empty();
	MagazineCapacityStorage.Empty();
	StoredAmmoStorage.Empty();
}
