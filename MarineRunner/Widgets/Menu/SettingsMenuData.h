// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingsMenuData.generated.h"

UENUM(BlueprintType)
enum ESettingToApply
{
	ESTA_MouseSensivity,
	ESTA_None
};

UENUM(BlueprintType)
enum ESettingsType
{
	EST_List,
	EST_KeyBindings,
	EST_Checked,
	EST_Category,
	EST_Slider
};

USTRUCT(BlueprintType)
struct FMenuSettings
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		FText SettingsName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingsType != ESettingsType::EST_Category"))
		TEnumAsByte<ESettingToApply> SettingToApply = ESTA_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		TEnumAsByte<ESettingsType> SettingsType = EST_List;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingsType == ESettingsType::EST_List"))
		TArray<FText> SettingsApplyTextes = { FText(FText::FromString("Low")),  FText(FText::FromString("Medium")),  FText(FText::FromString("High")),  FText(FText::FromString("Ultra")), };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingsType == ESettingsType::EST_List"))
		int32 CurrentSettingsListValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings")
		float CurrentSettingsValue = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingsType == ESettingsType::EST_KeyBindings"))
		FText SettingsKeyBindingsValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingsType == ESettingsType::EST_Checked"))
		bool bChecked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Settings", meta = (EditCondition = "SettingsType == ESettingsType::EST_Slider"))
		FFloatRange RangeOfSlider = FFloatRange(0.f, 100.f);
};
/**
 * 
 */
class MARINERUNNER_API SettingsMenuData
{

};
