// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/SaveGame/SaveGameJsonFile.h"

void UMarineRunnerGameInstance::Init()
{
	Super::Init();

	LoadCustomSavedSettingsFromConfig();
	LoadSoundsVolumeFromConfig(CustomSavedSettings);
}

#pragma region ///////// SAVING/LOADIGN///////////////
void UMarineRunnerGameInstance::SetSaveNumberAccordingToNumOfFiles(const FString& WildCard)
{
	const int32 NumberOfFilesInOneSave = 3; //.json, .png, .sav
	CurrentSaveNumber = GetAllFilesWithName(WildCard) / NumberOfFilesInOneSave;
}

void UMarineRunnerGameInstance::LoadCustomSavedSettingsFromConfig()
{
	FString ConfigPath = FPaths::GeneratedConfigDir() + CustomSavedSettingsConfigPath;
	bWasJsonDeserialized = USaveGameJsonFile::ReadJson(ConfigPath, SavedDataJsonFile);
	if (bWasJsonDeserialized == false)
		return;

	for (FSettingSavedInJsonFile& CurrentSavedSetting : CustomSavedSettings)
	{
		if (SavedDataJsonFile->TryGetField(CurrentSavedSetting.FieldName) == nullptr)
			continue;

		CurrentSavedSetting.FieldValue = SavedDataJsonFile->GetNumberField(CurrentSavedSetting.FieldName);
	}
}

void UMarineRunnerGameInstance::SaveCustomSavedSettingsToConfig()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	for (FSettingSavedInJsonFile& CurrentSavedSetting : CustomSavedSettings)
	{
		JsonObject->SetNumberField(CurrentSavedSetting.FieldName, CurrentSavedSetting.FieldValue);
	}

	FString ConfigPath = FPaths::GeneratedConfigDir() + CustomSavedSettingsConfigPath;
	USaveGameJsonFile::WriteJson(JsonObject, ConfigPath);
}

void UMarineRunnerGameInstance::FindSavedValueAccordingToName(const FString& SavedSettingName, float& Value)
{
	FSettingSavedInJsonFile NewStruct = FSettingSavedInJsonFile(SavedSettingName, 0.f);
	int32 FoundItemIndex = NewStruct.GetItemIndexInGivenArray(CustomSavedSettings);

	if (FoundItemIndex == INDEX_NONE)
		return;

	Value = CustomSavedSettings[FoundItemIndex].FieldValue;
}

void UMarineRunnerGameInstance::ReplaceValueInSavedSettingByName(float NewValue, const FString& SavedSettingName)
{
	FSettingSavedInJsonFile NewStruct = FSettingSavedInJsonFile(SavedSettingName, NewValue);
	int32 FoundItemIndex = NewStruct.GetItemIndexInGivenArray(CustomSavedSettings);

	if (FoundItemIndex == INDEX_NONE)
		return;

	CustomSavedSettings[FoundItemIndex].FieldValue = NewValue;
}
#pragma endregion

void UMarineRunnerGameInstance::AddNewDetectedEnemy(AActor* NewEnemy)
{
	DetectedPlayerEnemies.AddUnique(NewEnemy);

	if (DetectedPlayerEnemies.Num() > 0 && bIsDetectedByEnemies == false)
	{
		bIsDetectedByEnemies = true;
		ChangeBackgroundMusic(EMT_Combat);
	}
}

void UMarineRunnerGameInstance::RemoveDetectedEnemy(AActor* NewEnemy)
{
	DetectedPlayerEnemies.Remove(NewEnemy);
	if (DetectedPlayerEnemies.Num() == 0 && bIsDetectedByEnemies == true)
	{
		bIsDetectedByEnemies = false;
		ChangeBackgroundMusic(EMT_Exploration);
	}
}

void UMarineRunnerGameInstance::ResetDetectedEnemy()
{
	bIsDetectedByEnemies = false;
	DetectedPlayerEnemies.Empty();

	if (CurrentMusicType == EMusicType::EMT_Combat)
		ChangeBackgroundMusic(EMusicType::EMT_Exploration);
}

#pragma region /////////////////// BACKGROUND MUSIC /////////////////////////

void UMarineRunnerGameInstance::SpawnBackgroundMusic(USoundBase* SoundToSpawn)
{
	if (IsValid(SoundToSpawn) == false || IsValid(GetWorld()) == false)
		return;

	CurrentPlayingMusic = UGameplayStatics::SpawnSound2D(GetWorld(), SoundToSpawn, 1.f, 1.f, 0.f, nullptr, true);

	if (IsValid(CurrentPlayingMusic) == false)
		return;

	CurrentPlayingMusic->FadeIn(NewMusicFadeIn);
}

void UMarineRunnerGameInstance::ChangeBackgroundMusic(EMusicType MusicType, bool bIgnoreFadeOut)
{
	if (bIsDetectedByEnemies == true && MusicType != EMusicType::EMT_Combat)
		return;

	CurrentMusicType = MusicType;

	if (bIgnoreFadeOut == true)
	{
		if (IsValid(CurrentPlayingMusic))
			CurrentPlayingMusic->SetPaused(true);

		ChangeMusicAfterFadeOut();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(BackgroundMusicHandle, this, &UMarineRunnerGameInstance::ChangeMusicAfterFadeOut, OldMusicFadeOut + 0.01f, false);

		if (IsValid(CurrentPlayingMusic))
			CurrentPlayingMusic->FadeOut(OldMusicFadeOut, 0.f);
	}
}

void UMarineRunnerGameInstance::ChangeMusicAfterFadeOut()
{
	if (CurrentMusicType == EMT_Combat)
	{
		SpawnBackgroundMusic(CombatMusic);
		return;
	}

	if (CurrentMusicType != EMT_Exploration)
		return;

	if (IsValid(CurrentPlayingMusic) == true)
	{
		if (CurrentPlayingMusic->GetSound() == CurrentExplorationMusic)
		{
			CurrentPlayingMusic->SetPaused(false);
			return;
		}
	}

	SpawnBackgroundMusic(CurrentExplorationMusic);
}