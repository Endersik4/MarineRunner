// Copyright Adam Bartela.All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SettingsInformationData.h"

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
		TObjectPtr<class USettingsMenuWidget> SettingMenuWidget;
	UPROPERTY(EditAnywhere, Category = "Settings Menu")
		FString FunctionNameToApply;

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
		TObjectPtr<class UMarineRunnerGameInstance> GameInstance;
};
