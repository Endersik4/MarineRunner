// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

USaveMarineRunner::USaveMarineRunner()
{
	CurrentHealthSaved = 0;
	ClearTMapValues();
}

void USaveMarineRunner::SaveGame(float CurrentHealth, AGun* CurrentMarineGun, TMap < int32, AGun* > CurrentWeaponsStorage, TMap<FString, FItemStruct> CurrentInventory_ItemsSaved)
{
	ClearTMapValues();
	CurrentHealthSaved = CurrentHealth;

	Inventory_ItemsSaved = CurrentInventory_ItemsSaved;

	CurrentWeaponsStorage.GenerateValueArray(WeaponsSaved);
	WeaponsStorageSaved = CurrentWeaponsStorage;
	MarineGun = CurrentMarineGun;
	for (auto& Storage : WeaponsSaved)
	{
		MagazineCapacityStorage.Add(Storage, Storage->GetMagazineCapacity());
	}
}

void USaveMarineRunner::LoadGame(class AMarineCharacter* MarinePawn)
{
	if (MarinePawn == nullptr) return;

	MarinePawn->SetHealth(CurrentHealthSaved);
	MarinePawn->SetQuickSelect(WeaponsStorageSaved);
	MarinePawn->GetInventoryComponent()->Inventory_Items = Inventory_ItemsSaved;

	if (!MarineGun) return;

	MarinePawn->SetGun(MarineGun);

	for (auto& CurrentWeapon : WeaponsSaved)
	{
		if (MagazineCapacityStorage.Find(CurrentWeapon))
		{
			CurrentWeapon->SetMagazineCapacity(*MagazineCapacityStorage.Find(CurrentWeapon));
		}

		CurrentWeapon->SetItemFromInventory(Inventory_ItemsSaved.Find(CurrentWeapon->GetAmmoName()));
		CurrentWeapon->SetMarinePawn(MarinePawn);

		if (MarinePawn->GetGun() == CurrentWeapon)
		{	
			CurrentWeapon->EquipWeapon(false);
			CurrentWeapon->SetGunSwayWhileMovingTimer();
		}
		else
		{
			CurrentWeapon->EquipWeapon(false, false);
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
	Inventory_ItemsSaved.Empty();
}
