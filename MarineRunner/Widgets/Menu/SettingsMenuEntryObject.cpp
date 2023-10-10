// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuEntryObject.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

USettingsMenuEntryObject::USettingsMenuEntryObject()
{
}

void USettingsMenuEntryObject::SetVariablesToCurrent()
{
	MenuSettingsData.bEntryWidgetEnabled = true;

	QualityValue();
	SliderValue();
	CheckBoxValue();
	SetResolutionSetting();
	SetFullscreenModeSetting();
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

	MenuSettingsData.SliderCurrentValue = UKismetSystemLibrary::GetConsoleVariableFloatValue(MenuSettingsData.SubSettingFunctionName);
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

void USettingsMenuEntryObject::SetResolutionSetting()
{
	if (MenuSettingsData.SubSettingType != EST_SetResolution)
		return;

	FillSupportedResolutions();

	FIntPoint CurrentResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
	MenuSettingsData.QualityCurrentValue = MenuSettingsData.SupportedResolutionsList.Find(CurrentResolution);
	if (MenuSettingsData.QualityCurrentValue == INDEX_NONE) MenuSettingsData.QualityCurrentValue = 0;
}

void USettingsMenuEntryObject::FillSupportedResolutions()
{
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(MenuSettingsData.SupportedResolutionsList);

	MenuSettingsData.QualityTypes.Empty();
	for (const FIntPoint& CurrResolution : MenuSettingsData.SupportedResolutionsList)
	{
		const FString NewResolution = FString::FromInt(CurrResolution.X) + "x" + FString::FromInt(CurrResolution.Y);
		MenuSettingsData.QualityTypes.Add(NewResolution);
	}
}

void USettingsMenuEntryObject::SetFullscreenModeSetting()
{
	if (MenuSettingsData.SubSettingType != EST_SetFullscreen)
		return;

	MenuSettingsData.QualityCurrentValue = UGameUserSettings::GetGameUserSettings()->GetFullscreenMode();
}

bool USettingsMenuEntryObject::WasValueLoadedFromJsonFile(float& Value)
{
	if (IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("DZIALA game instance"));
	}
	else 
		UE_LOG(LogTemp, Warning, TEXT("nie DZIALA game isntance"));

	if (MenuSettingsData.bSaveValueToGameInstance == false || IsValid(GameInstance) == false)
		return false;
	UE_LOG(LogTemp, Warning, TEXT("DZIALA"));
	GameInstance->SetValueBySavedSettingName(MenuSettingsData.SavedValueName, Value);

	return true;
}

bool USettingsMenuEntryObject::WasValueLoadedFromJsonFile(int32& Value)
{
	if (MenuSettingsData.bSaveValueToGameInstance == false || IsValid(GameInstance) == false)
		return false;

	//GameInstance->SetValueBySavedSettingName(MenuSettingsData.SavedValueName, Value);

	return true;
}

bool USettingsMenuEntryObject::WasValueLoadedFromJsonFile(bool& Value)
{
	if (MenuSettingsData.bSaveValueToGameInstance == false || IsValid(GameInstance) == false)
		return false;

	//GameInstance->SetValueBySavedSettingName(MenuSettingsData.SavedValueName, Value);

	return true;
}

