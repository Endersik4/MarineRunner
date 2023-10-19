// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/SaveGame/SaveGameJsonFile.h"

USaveMarineRunner::USaveMarineRunner()
{
	
}
void USaveMarineRunner::PrepareSaveGame(const FString& NewSaveName, bool bNewAddSaveNumber)
{
	OriginalSaveName = NewSaveName;
	bAddSaveNumber = bNewAddSaveNumber;
}

FString USaveMarineRunner::GetSaveGameName()
{
	return OriginalSaveName + (bAddSaveNumber ? FString::FromInt(SaveNumber) : "");
}

#pragma region ///////// SAVING ////////////
void USaveMarineRunner::CopySaveInfoToCurrentGameInstance(UWorld* CurrentWorld)
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(CurrentWorld));
	if (IsValid(GameInstance) == false)
		return;

	GameInstance->CurrentSaveNumber++;
	SaveNumber = GameInstance->CurrentSaveNumber;

	GameInstance->SlotSaveGameNameToLoad = GetSaveGameName();
	GameInstance->TotalPlayTimeInSeconds += UKismetSystemLibrary::GetGameTimeInSeconds(CurrentWorld);

	TotalPlayTimeInSeconds = GameInstance->TotalPlayTimeInSeconds;
}

void USaveMarineRunner::SaveGame(float CurrentHealth, AGun* CurrentMarineGun, TMap < int32, AGun* > CurrentWeaponsStorage, TMap<FString, FItemStruct> CurrentInventory_ItemsSaved)
{
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

FString USaveMarineRunner::TakeSaveScreenshot(APlayerController* PlayerController)
{
	if (IsValid(PlayerController) == false) return "0";

	FString ScreenshotName = GetSaveGameName() +".jpg";
	FString SaveGamesDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + GetSaveGameName() + "/" + ScreenshotName;
	FString TakeScreenShotCommand = "HighResShot 320x180 filename=\"" + SaveGamesDir + "\"";

	PlayerController->ConsoleCommand(TakeScreenShotCommand);
	return SaveGamesDir;
}

FString USaveMarineRunner::ConvertCurrentDateToText()
{
	const FString& CurrentTimeText_Minutes = FDateTime::Now().GetMinute() < 10 ? "0" + FString::FromInt(FDateTime::Now().GetMinute()) : FString::FromInt(FDateTime::Now().GetMinute());
	const FString& CurrentTimeText = FString::FromInt(FDateTime::Now().GetHour()) + ":" + CurrentTimeText_Minutes;

	const FString& SaveDateText = CurrentTimeText + " " + FString::FromInt(FDateTime::Now().GetDay()) + "/" + FString::FromInt(FDateTime::Now().GetMonth()) + "/" + FString::FromInt(FDateTime::Now().GetYear());
	return SaveDateText;
}

void USaveMarineRunner::MakeJsonFileWithSaveInfo(APlayerController* PlayerController, const FString& CurrentLevelName)
{
	const FString& PathToScreenshot = TakeSaveScreenshot(PlayerController);
	const FString& SaveDateText = ConvertCurrentDateToText();

	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + GetSaveGameName() + "/" + GetSaveGameName() + ".json";

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("SavedDataName", GetSaveGameName());
	JsonObject->SetNumberField("SavedDataNumber", SaveNumber);
	JsonObject->SetStringField("PathToThumbnail", PathToScreenshot);
	JsonObject->SetStringField("SavedGameDate", ConvertCurrentDateToText());
	JsonObject->SetStringField("SavedLevelName", CurrentLevelName);
	JsonObject->SetNumberField("TotalPlayTime", TotalPlayTimeInSeconds);

	USaveGameJsonFile::WriteJson(JsonObject, FilePath);
}

#pragma endregion

#pragma region ////////// LOADING //////////////
void USaveMarineRunner::LoadGame(AMarineCharacter* MarinePawn, UMarineRunnerGameInstance* GameInstance)
{
	if (IsValid(MarinePawn) == false) return;

	MarinePawn->SetHealth(CurrentHealthSaved);
	MarinePawn->SetQuickSelect(WeaponsStorageSaved);
	MarinePawn->GetInventoryComponent()->Inventory_Items = Inventory_ItemsSaved;

	LoadEquipedGuns(MarinePawn);

	if (IsValid(GameInstance) == false) return;

	GameInstance->SetSaveNumberAccordingToNumOfFiles();
	GameInstance->TotalPlayTimeInSeconds = TotalPlayTimeInSeconds;
}

void USaveMarineRunner::LoadEquipedGuns(AMarineCharacter* MarinePawn)
{
	if (!MarineGun) return;

	MarinePawn->GetWeaponHandlerComponent()->SetGun(MarineGun);

	for (auto& CurrentWeapon : WeaponsSaved)
	{
		if (MagazineCapacityStorage.Find(CurrentWeapon))
		{
			CurrentWeapon->SetMagazineCapacity(*MagazineCapacityStorage.Find(CurrentWeapon));
		}

		CurrentWeapon->SetItemFromInventory(Inventory_ItemsSaved.Find(CurrentWeapon->GetAmmoName()));
		CurrentWeapon->SetMarinePawn(MarinePawn);

		if (MarinePawn->GetWeaponHandlerComponent()->GetGun() == CurrentWeapon)
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