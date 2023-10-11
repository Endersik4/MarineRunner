// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

#include "MarineRunner/SaveGame/SaveGameJsonFile.h"


void UMarineRunnerGameInstance::Init()
{
	Super::Init();
	SetSaveNumberAccordingToNumOfFiles();

	LoadCustomSavedSettingsFromConfig();
	LoadSoundsVolumeFromConfig(CustomSavedSettings);
}

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
