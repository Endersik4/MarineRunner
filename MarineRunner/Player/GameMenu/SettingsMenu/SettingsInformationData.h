// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SettingsInformationData.generated.h"

UENUM(BlueprintType)
enum ESettingsType
{
	EST_Quality, // Quality - player can choose quality from QualityTypes where Index mean quality (eg. 0 - low, 1 - medium...)
	EST_KeyMapping, // KeyMapping - when player click on KeyMappingName then wait for player to press any button and assign new key to the bindings
	EST_OnOff, // OnOff - Turn On/Off (CheckBox)
	EST_Category, // Category - SubSettingName will be in the center of the SettingsMenuListEntry with diffrent color, no action
	EST_SliderValue, // SlideValue - Slider with Max/Min Range  
	EST_SetResolution,
	EST_SetFullscreen,
};

UENUM(BlueprintType)
enum ESettingApplyType
{
	ESAT_FunctionInCMD,
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

	// If true, the value will be stored in the config or loaded from the config
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings|Save Config")
	bool bSaveValueToGameInstance = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings|Save Config", meta = (EditCondition = "bSaveValueToGameInstance", EditConditionHides))
	FString SavedValueName;

	// If SettingApplyType is set to ESAT_FunctionInCMD, this field holds the function name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings",
		meta = (EditCondition = "SettingApplyType == ESettingApplyType::ESAT_FunctionInCMD", EditConditionHides))
	FString SubSettingFunctionName = "sg.TextureQuality";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingApplyType == ESettingApplyType::ESAT_Sounds", EditConditionHides))
	TObjectPtr<USoundClass> SoundClassToChangeVolume = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingApplyType == ESettingApplyType::ESAT_Sounds", EditConditionHides))
	TObjectPtr<USoundMix> SoundMixClassToChangeVolume = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
	TEnumAsByte<ESettingsType> SubSettingType = EST_Quality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings",
		meta = (EditCondition = "SubSettingType == ESettingsType::EST_Quality || SubSettingType == ESettingsType::EST_SetFullscreen", EditConditionHides))
	TArray<FString> QualityTypes = { "Low",  "Medium",  "High", "Ultra" };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings",
		meta = (EditCondition = "SubSettingType == ESettingsType::EST_Quality || SubSettingType == ESettingsType::EST_SetFullscreen", EditConditionHides))
	int32 QualityCurrentValue = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_SetFullscreen", EditConditionHides))
	TArray<TEnumAsByte<EWindowMode::Type>> AllWindowTypes = { EWindowMode::Type::Fullscreen ,EWindowMode::Type::WindowedFullscreen ,EWindowMode::Type::Windowed };

	TArray<FIntPoint> SupportedResolutionsList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_KeyMapping", EditConditionHides))
	FName KeyMappingName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SubSettingType == ESettingsType::EST_KeyMapping", EditConditionHides))
	int32 IndexOfKey = 0;

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
