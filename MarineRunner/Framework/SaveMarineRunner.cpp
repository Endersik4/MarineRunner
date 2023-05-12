// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

USaveMarineRunner::USaveMarineRunner()
{
	AmountOfFirstAidKits = 0;
	CurrentHealth = 0;
	ClearTMapValues();
}

void USaveMarineRunner::SaveGame(int32 NewAmountOfFirstAidKits, float NewCurrentHealth, TArray<AGun*> WeaponsStorage)
{
	ClearTMapValues();
	AmountOfFirstAidKits = NewAmountOfFirstAidKits;
	CurrentHealth = NewCurrentHealth;
	Weapons = WeaponsStorage;
	for (auto& Storage : WeaponsStorage)
	{
		MagazineCapacityStorage.Add(Storage, Storage->GetMagazineCapacity());
		StoredAmmoStorage.Add(Storage, Storage->GetStoredAmmo());
	}
}

void USaveMarineRunner::LoadGame(class AMarineCharacter* MarinePawn)
{
	if (MarinePawn == nullptr) return;

	MarinePawn->SetFirstAidKits(AmountOfFirstAidKits);
	MarinePawn->SetHealth(CurrentHealth);

	for (auto& CurrentWeapon : Weapons)
	{
		if (MagazineCapacityStorage.Find(CurrentWeapon))
		{
			CurrentWeapon->SetMagazineCapacity(*MagazineCapacityStorage.Find(CurrentWeapon));
		}
		if (StoredAmmoStorage.Find(CurrentWeapon))
		{
			CurrentWeapon->SetStoredAmmo(*StoredAmmoStorage.Find(CurrentWeapon));
		}
		if (MarinePawn->GetGun() == CurrentWeapon) CurrentWeapon->SetWeaponInHud(true);
	}
}

void USaveMarineRunner::ClearTMapValues()
{
	Weapons.Empty();
	MagazineCapacityStorage.Empty();
	StoredAmmoStorage.Empty();
}
