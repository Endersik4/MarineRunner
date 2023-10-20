// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/SaveGame/SaveGameJsonFile.h"


void UMarineRunnerGameInstance::Init()
{
	Super::Init();
	SetSaveNumberAccordingToNumOfFiles();

	LoadCustomSavedSettingsFromConfig();
	LoadSoundsVolumeFromConfig(CustomSavedSettings);

	SpawnBackgroundMusicFirstTime();
}

#pragma region ///////// SAVING/LOADIGN///////////////
void UMarineRunnerGameInstance::SetSaveNumberAccordingToNumOfFiles()
{
	FString WildCard = "*ManualSave*";
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

void UMarineRunnerGameInstance::SetValueBySavedSettingName(const FString& SavedSettingName, float& Value)
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

void UMarineRunnerGameInstance::AddNewDetectedEnemy(AActor* NewEnemy, bool bIsEnemyDead)
{
	if (bIsEnemyDead)
		return;
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

#pragma region /////////////////// BACKGROUND MUSIC /////////////////////////
void UMarineRunnerGameInstance::SpawnBackgroundMusicFirstTime()
{
	FTimerDelegate MusicDel;
	MusicDel.BindUFunction(this, FName("SpawnBackgroundMusic"), ExplorationMusic);
	GetWorld()->GetTimerManager().SetTimer(BackgroundMusicHandle, MusicDel, 0.2f, false);
}

void UMarineRunnerGameInstance::SpawnBackgroundMusic(USoundBase* SoundToSpawn)
{
	if (SoundToSpawn == nullptr)
		return;
	CurrentPlayingMusic = UGameplayStatics::SpawnSound2D(GetWorld(), SoundToSpawn, 1.f, 1.f, 0.f, nullptr, true);
	CurrentPlayingMusic->FadeIn(2.f, 1.f);
}

void UMarineRunnerGameInstance::ChangeBackgroundMusic(EMusicType MusicType, bool bIgnoreFadeOut)
{
	CurrentMusicType = MusicType;

	GetWorld()->GetTimerManager().SetTimer(BackgroundMusicHandle, this, &UMarineRunnerGameInstance::ChangeMusicAfterFadeOut, 1.81f, false);

	if (bIgnoreFadeOut == true)
		return;

	if (IsValid(CurrentPlayingMusic) == false)
		return;

	CurrentPlayingMusic->FadeOut(1.8f, 0.f);
}

void UMarineRunnerGameInstance::ChangeMusicAfterFadeOut()
{
	if (CurrentMusicType == EMT_Combat)
		SpawnBackgroundMusic(CombatMusic);
	else if (CurrentMusicType == EMT_PauseMusic)
	{
		CurrentPlayingMusic->Stop();
		return;
	}
	else SpawnBackgroundMusic(ExplorationMusic);
}