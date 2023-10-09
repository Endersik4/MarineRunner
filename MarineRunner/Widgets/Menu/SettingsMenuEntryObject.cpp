// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuEntryObject.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/SaveGame/SaveGameJsonFile.h"


USettingsMenuEntryObject::USettingsMenuEntryObject()
{
}

void USettingsMenuEntryObject::SetVariablesToCurrent()
{
	MenuSettingsData.bEntryWidgetEnabled = true;

	if (MenuSettingsData.bSaveValueToConfig == true)
	{
		FString ConfigPath = FPaths::GeneratedConfigDir();
		ConfigPath += "MarineRunner/Config/Settings.json";
		bWasJsonDeserialized = USaveGameJsonFile::ReadJson(ConfigPath, SavedDataJsonFile);
	}

	QualityValue();
	SliderValue();
	CheckBoxValue();
}

void USettingsMenuEntryObject::QualityValue()
{
	if (MenuSettingsData.SubSettingType != EST_Quality) 
		return;

	if (WasValueLoadedFromJsonFile(MenuSettingsData.QualityCurrentValue))
		return;

	if (MenuSettingsData.SettingApplyType != ESAT_FunctionInCMD)
		return;

	MenuSettingsData.QualityCurrentValue = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName);
}
void USettingsMenuEntryObject::SliderValue()
{
	if (MenuSettingsData.SubSettingType != EST_SliderValue) 
		return;

	if (WasValueLoadedFromJsonFile(MenuSettingsData.SliderCurrentValue))
		return;

	if (MenuSettingsData.SettingApplyType != ESAT_FunctionInCMD) 
		return;

	MenuSettingsData.SliderCurrentValue = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName);
}
void USettingsMenuEntryObject::CheckBoxValue() 
{
	if (MenuSettingsData.SubSettingType != EST_OnOff) 
		return;

	if (WasValueLoadedFromJsonFile(MenuSettingsData.bSettingEnabled))
		return;

	if (MenuSettingsData.SettingApplyType != ESAT_FunctionInCMD)
		return;

	// sometimes value from ConsoleVariable can be -1 so this line prevent form wrong boolean value;
	MenuSettingsData.bSettingEnabled = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName) <= 0 ? false : true;
}

bool USettingsMenuEntryObject::WasValueLoadedFromJsonFile(float& Value)
{
	if (MenuSettingsData.bSaveValueToConfig == true)
	{
		if (bWasJsonDeserialized && SavedDataJsonFile->TryGetField(MenuSettingsData.SavedValueName))
		{		
			Value = SavedDataJsonFile->GetNumberField(MenuSettingsData.SavedValueName);
		}

		return true;
	}

	return false;
}

bool USettingsMenuEntryObject::WasValueLoadedFromJsonFile(int32& Value)
{
	if (MenuSettingsData.bSaveValueToConfig == true)
	{
		if (bWasJsonDeserialized && SavedDataJsonFile->TryGetField(MenuSettingsData.SavedValueName))
		{
			Value = SavedDataJsonFile->GetNumberField(MenuSettingsData.SavedValueName);
		}

		return true;
	}

	return false;
}

bool USettingsMenuEntryObject::WasValueLoadedFromJsonFile(bool& Value)
{
	if (MenuSettingsData.bSaveValueToConfig == true)
	{
		if (bWasJsonDeserialized && SavedDataJsonFile->TryGetField(MenuSettingsData.SavedValueName))
		{
			Value = SavedDataJsonFile->GetBoolField(MenuSettingsData.SavedValueName);
		}

		return true;
	}

	return false;
}

