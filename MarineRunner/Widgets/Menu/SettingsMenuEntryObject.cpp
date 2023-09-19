// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuEntryObject.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"

USettingsMenuEntryObject::USettingsMenuEntryObject()
{
	SetVariablesToCurrent();
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
	if (MenuSettingsData.SubSettingType != EST_Quality) return;

	MenuSettingsData.QualityCurrentValue = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName);

}
void USettingsMenuEntryObject::SliderValue()
{
	if (MenuSettingsData.SubSettingType != EST_SliderValue) return;

	if (MenuSettingsData.SettingApplyType == ESAT_MouseSens)
	{
		AMarinePlayerController* PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (IsValid(PC))
			MenuSettingsData.SliderCurrentValue = PC->GetMouseSensitivity();
	}
}
void USettingsMenuEntryObject::CheckBoxValue() 
{
	if (MenuSettingsData.SubSettingType != EST_OnOff) return;

	// sometimes value from ConsoleVariable can be -1 so this line prevent form wrong boolean value
	MenuSettingsData.bSettingEnabled = UKismetSystemLibrary::GetConsoleVariableIntValue(MenuSettingsData.SubSettingFunctionName) <= 0 ? false : true;
}
