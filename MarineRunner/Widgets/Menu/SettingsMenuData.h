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

USTRUCT(BlueprintType)
struct FMenuSettings
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		FText SubSettingName = FText::FromString("-textures quality-");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		TEnumAsByte<ESettingApplyType> SettingApplyType = ESAT_FunctionInCMD;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SliderValue", EditConditionHides))
		float SliderCurrentValue = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SliderValue", EditConditionHides))
		int32 DecimalNumbers = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SliderValue", EditConditionHides))
		FFloatRange RangeOfSlider = FFloatRange(0.f, 100.f);
};
/**
 * 
 */
class MARINERUNNER_API SettingsMenuData
{

};
