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

	FTimerHandle gowno;
	GetWorld()->GetTimerManager().SetTimer(gowno, this, &UMarineRunnerGameInstance::SpawnMusic, 1.f, false);
}


void UMarineRunnerGameInstance::SpawnMusic()
{
	if (ExplorationMusic == nullptr)
		return;
	CurrentPlayingMusic = UGameplayStatics::SpawnSound2D(GetWorld(), ExplorationMusic);
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

void UMarineRunnerGameInstance::AddNewDetectedEnemy(AActor* NewEnemy)
{
	DetectedPlayerEnemies.Add(NewEnemy);
	if (DetectedPlayerEnemies.Num() > 0 && bIsDetectedByEnemies == false)
	{
		bIsDetectedByEnemies = true;
		UE_LOG(LogTemp, Warning, TEXT("DETWECTED"));
		ChangeBackgroundMusic(EMT_Combat);
	}
}

void UMarineRunnerGameInstance::RemoveDetectedEnemy(AActor* NewEnemy)
{
	if (DetectedPlayerEnemies.Find(NewEnemy) == INDEX_NONE)
		return;

	DetectedPlayerEnemies.Remove(NewEnemy);
	if (DetectedPlayerEnemies.Num() < 1 && bIsDetectedByEnemies == true)
	{
		bIsDetectedByEnemies = false;
		UE_LOG(LogTemp, Error, TEXT("NOT DETWECTED"));
		ChangeBackgroundMusic(EMT_Exploration);
	}
}

void UMarineRunnerGameInstance::ChangeBackgroundMusic(EMusicType MusicType)
{
	if (IsValid(CurrentPlayingMusic) == false)
		return;

	if (MusicType == EMT_Combat && CombatMusic)
		CurrentPlayingMusic->SetSound(CombatMusic);
	else if (MusicType == EMT_MainMenu && MainMenuMusic)
		CurrentPlayingMusic->SetSound(MainMenuMusic);
	else if (ExplorationMusic) CurrentPlayingMusic->SetSound(ExplorationMusic);
}
