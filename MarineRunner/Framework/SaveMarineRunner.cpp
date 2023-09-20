// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

USaveMarineRunner::USaveMarineRunner()
{
	CurrentHealthSaved = 0;
	ClearTMapValues();
}

#pragma region ///////// SAVING ////////////
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

	SaveNumber++;
}


FString USaveMarineRunner::GetSaveGameName()
{
	FString NewName = "ManualSave_" + FString::FromInt(SaveNumber);
	return FString(NewName);
}

FString USaveMarineRunner::TakeSaveScreenshot(APlayerController* PlayerController)
{
	if (IsValid(PlayerController) == false) return "0";

	FString ScreenshotName = GetSaveGameName()+".jpg";
	FString SaveGamesDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + ScreenshotName;
	FString TakeScreenShotCommand = "HighResShot 320x180 filename=\"" + SaveGamesDir + "\"";

	PlayerController->ConsoleCommand(TakeScreenShotCommand);
	return SaveGamesDir;
}

void USaveMarineRunner::MakeSaveMenuData(APlayerController* PlayerController)
{
	const FString& PathToScreenshot = TakeSaveScreenshot(PlayerController);
	FString ClockTime = FString::FromInt(FDateTime::Now().GetHour()) + ":" + FString::FromInt(FDateTime::Now().GetMinute());
	FString SaveDateText = ClockTime + " " + FString::FromInt(FDateTime::Now().GetDay()) + "/" + FString::FromInt(FDateTime::Now().GetMonth()) + "/" + FString::FromInt(FDateTime::Now().GetYear());
	
	FSaveDataMenuStruct NewSaveDataMenu = FSaveDataMenuStruct(GetSaveGameName(), PathToScreenshot, SaveDateText, 550);
	SavesData.Add(NewSaveDataMenu);
}

#pragma endregion

#pragma region ////////// LOADING //////////////
void USaveMarineRunner::LoadGame(AMarineCharacter* MarinePawn)
{
	if (MarinePawn == nullptr) return;

	MarinePawn->SetHealth(CurrentHealthSaved);
	MarinePawn->SetQuickSelect(WeaponsStorageSaved);
	MarinePawn->GetInventoryComponent()->Inventory_Items = Inventory_ItemsSaved;

	LoadEquipedGuns(MarinePawn);
}

void USaveMarineRunner::LoadEquipedGuns(AMarineCharacter* MarinePawn)
{
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
			CurrentWeapon->EquipWeapon();
			CurrentWeapon->SetGunSwayWhileMovingTimer();
		}
		else
		{
			CurrentWeapon->EquipWeapon(false);
			CurrentWeapon->SetGunSwayWhileMovingTimer(true);
			CurrentWeapon->SetActorHiddenInGame(true);
		}
		CurrentWeapon->SetCanGunSwayTick(true);
		CurrentWeapon->SetActorRelativeLocation(MarineGun->GetRelativeLocationInPawn());
	}
}
#pragma endregion

void USaveMarineRunner::ClearTMapValues()
{
	WeaponsSaved.Empty();
	MagazineCapacityStorage.Empty();
	Inventory_ItemsSaved.Empty();
}

