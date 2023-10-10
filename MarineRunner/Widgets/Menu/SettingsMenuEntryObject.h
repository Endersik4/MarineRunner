// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SettingsMenuData.h"
#include "SettingsMenuEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API USettingsMenuEntryObject : public UObject
{
	GENERATED_BODY()


public:

	USettingsMenuEntryObject();

	UPROPERTY(EditAnywhere, Category = "Settings Menu")
		FMenuSettings MenuSettingsData;
	UPROPERTY(EditAnywhere, Category = "Settings Menu")
		class USettingsMenuWidget* SettingMenuWidget;
	UPROPERTY(EditAnywhere, Category = "Settings Menu")
		FString FunctionNameToApply;

	void SetVariablesToCurrent();
	FORCEINLINE void SetGameInstance(class UMarineRunnerGameInstance* NewGameInstance) { GameInstance = NewGameInstance; }
private:
	void QualityValue();
	void SliderValue();
	void CheckBoxValue();

	// Custom Settings
	void SetResolutionSetting();
	void FillSupportedResolutions();
	void SetFullscreenModeSetting();

	class UMarineRunnerGameInstance* GameInstance;

	bool WasValueLoadedFromJsonFile(float& Value);
	bool WasValueLoadedFromJsonFile(int32& Value);
	bool WasValueLoadedFromJsonFile(bool& Value);

};
