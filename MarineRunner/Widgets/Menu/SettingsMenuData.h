// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingsMenuData.generated.h"

UENUM(BlueprintType)
enum ESettingsType
{
	EST_Quality, // Quality - player can choose quality from QualityTypes where Index mean quality (eg. 0 - low, 1 - medium...)
	EST_KeyMapping, // KeyMapping - when player click on KeyMappingActionName then wait for player to press any button and assign new key to the bindings
	EST_OnOff, // OnOff - Turn On/Off (CheckBox)
	EST_Category, // Category - SubSettingName will be in the center of the SettingsMenuListEntry with diffrent color, no action
	EST_SliderValue // SlideValue - Slider with Max/Min Range  
};

UENUM(BlueprintType)
enum ESettingApplyType
{
	ESAT_FunctionInCMD,
	ESAT_MouseSens,
	ESAT_Sounds,
	ESAT_None
};


// The FMenuSettings structure defines various types of game menu settings, such as texture quality, key mapping, volume, etc.
// Each setting has different parameters, such as name, application method, and type-specific values.
USTRUCT(BlueprintType)
struct FMenuSettings
{
	GENERATED_USTRUCT_BODY();

	// Represents the name displayed to describe the setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		FText SubSettingName = FText::FromString("-textures quality-");
	// Dictates whether the setting applies as a function in CMD or through sound, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		TEnumAsByte<ESettingApplyType> SettingApplyType = ESAT_FunctionInCMD;

	// If SettingApplyType is set to ESAT_FunctionInCMD, this field holds the function name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingApplyType == ESettingApplyType::ESAT_FunctionInCMD", EditConditionHides))
		FString SubSettingFunctionName = "sg.TextureQuality";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingApplyType == ESettingApplyType::ESAT_Sounds", EditConditionHides))
		USoundClass* SoundClassToChangeVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingApplyType == ESettingApplyType::ESAT_Sounds", EditConditionHides))
		USoundMix* SoundMixClassToChangeVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		TEnumAsByte<ESettingsType> SubSettingType = EST_Quality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_Quality", EditConditionHides))
		TArray<FText> QualityTypes = { FText(FText::FromString("Low")),  FText(FText::FromString("Medium")),  FText(FText::FromString("High")),  FText(FText::FromString("Ultra")), };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_Quality", EditConditionHides))
		int32 QualityCurrentValue = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_KeyMapping", EditConditionHides))
		FName KeyMappingActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_OnOff", EditConditionHides))
		bool bSettingEnabled = false;
	// This flag indicates whether the setting serves as an object that connects with other settings, if the SubSettingType is set to EST_OnOff.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_OnOff", EditConditionHides))
		bool bIsItObjectThatConnects = false;

	// Acts as a flag indicating if the setting is linked to other settings.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "!bIsItObjectThatConnects", EditConditionHides))
		bool bIsConnectedToOtherSettings = false;

	// Stores the present value of the slider if SubSettingType is set to EST_SliderValue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SliderValue", EditConditionHides))
		float SliderCurrentValue = 0.7f;
	// Specifies the number of decimal places for the slider value. if DecimalNumbers == 0 then Slider value will be integer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SliderValue", EditConditionHides))
		int32 DecimalNumbers = 2;
	// Defines the range of values for the slider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SliderValue", EditConditionHides))
		FFloatRange RangeOfSlider = FFloatRange(0.f, 100.f);

	// Is a flag that determines if the entry widget is enabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		bool bEntryWidgetEnabled = true;
};
/**
 * 
 */
class MARINERUNNER_API SettingsMenuData
{

};
