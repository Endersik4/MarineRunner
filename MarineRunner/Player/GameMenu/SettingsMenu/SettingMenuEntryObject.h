// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SettingsInformationData.h"

#include "SettingMenuEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API USettingMenuEntryObject : public UObject
{
	GENERATED_BODY()
public:

	USettingMenuEntryObject();

	UPROPERTY(EditAnywhere, Category = "Settings Menu")
	FMenuSettings MenuSettingsData = FMenuSettings();
	UPROPERTY(EditAnywhere, Category = "Settings Menu")
	TObjectPtr<class USettingsMenuWidget> SettingMenuWidget = nullptr;
	UPROPERTY(EditAnywhere, Category = "Settings Menu")
	FString FunctionNameToApply = FString("");

	void SetVariablesToCurrent();
	FORCEINLINE void SetGameInstance(class UMarineRunnerGameInstance* NewGameInstance) { GameInstance = NewGameInstance; }
private:
	void QualityValue();
	void SliderValue();
	void CheckBoxValue();

	void SetResolutionSetting();
	void FillSupportedResolutions();
	void SetFullscreenModeSetting();

	bool WasValueLoadedFromJsonFile(float& Value);
	bool WasValueLoadedFromJsonFile(int32& Value);
	bool WasValueLoadedFromJsonFile(bool& Value);

	UPROPERTY(Transient)
	TObjectPtr<class UMarineRunnerGameInstance> GameInstance = nullptr;
};
