// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuEntryObject.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

USettingsMenuEntryObject::USettingsMenuEntryObject()
{
}

void USettingsMenuEntryObject::SetVariablesToCurrent()
{
	MenuSettingsData.bEntryWidgetEnabled = true;
	QualityValue();
	SliderValue();
	CheckBoxValue();
}

void USettingsMenuEntryObject::QualityValue()
{
	if (MenuSettingsData.SubSettingType != EST_Quality || MenuSettingsData.SettingApplyType != ESAT_FunctionInCMD) return;

	MenuSettingsData.QualityCurrentValue = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName);
}
void USettingsMenuEntryObject::SliderValue()
{
	if (MenuSettingsData.SubSettingType != EST_SliderValue) return;

	if (MenuSettingsData.SettingApplyType == ESAT_MouseSens)
	{
		MenuSettingsData.SliderCurrentValue = GetMouseSensitivityFromMenuSettingsData();

		return;
	}

	if (MenuSettingsData.SettingApplyType != ESAT_FunctionInCMD) return;

	MenuSettingsData.SliderCurrentValue = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName);
}
void USettingsMenuEntryObject::CheckBoxValue() 
{
	if (MenuSettingsData.SubSettingType != EST_OnOff || MenuSettingsData.SettingApplyType != ESAT_FunctionInCMD) return;

	// sometimes value from ConsoleVariable can be -1 so this line prevent form wrong boolean value;
	MenuSettingsData.bSettingEnabled = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName) <= 0 ? false : true;
}

float USettingsMenuEntryObject::GetMouseSensitivityFromMenuSettingsData()
{
	AMarineCharacter* MarinePawn = Cast<AMarineCharacter>(PlayerPawn);

	if (IsValid(MarinePawn) == false)
		return 0.7f;

	if (MenuSettingsData.MouseSensitivityType == EMST_16xScope)
	{
		return MarinePawn->GetMouseSensitivityWhenScope(3);
	}
	else if (MenuSettingsData.MouseSensitivityType == EMST_8xScope)
	{
		return MarinePawn->GetMouseSensitivityWhenScope(2);
	}
	else if (MenuSettingsData.MouseSensitivityType == EMST_4xScope)
	{
		return MarinePawn->GetMouseSensitivityWhenScope(1);
	}
	else if (MenuSettingsData.MouseSensitivityType == EMST_2xScope)
	{
		return MarinePawn->GetMouseSensitivityWhenScope(0);
	}
	else
	{
		return MarinePawn->GetMouseSensitivity();
	} 
}
