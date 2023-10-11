// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MarineRunnerGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSettingSavedInJsonFile {
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FieldName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FieldValue;

	FSettingSavedInJsonFile() {
		FieldName = "";
		FieldValue = 0.f;
	}

	FSettingSavedInJsonFile(FString NewFieldName, float NewFieldValue)
	{
		FieldName = NewFieldName;
		FieldValue = NewFieldValue;
	}

	FORCEINLINE bool operator==(const FSettingSavedInJsonFile& SavedSetting) const
	{
		return FieldName == SavedSetting.FieldName;
	}

	int32 GetItemIndexInGivenArray(const TArray<FSettingSavedInJsonFile>& Array)
	{
		int32 Index = 0;
		for (const FSettingSavedInJsonFile& CurrentStruct : Array)
		{
			if (*this == CurrentStruct)
			{
				return Index;
			}
			Index++;
		}
		return INDEX_NONE;
	}
};

UCLASS()
class MARINERUNNER_API UMarineRunnerGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
		virtual void Init() override;
public:
	UPROPERTY(EditAnywhere)
		FString CustomSavedSettingsConfigPath = "MarineRunner/Config/Settings.json";
	UPROPERTY(EditAnywhere)
		FString SlotSaveGameNameToLoad = "MySlot";
	UPROPERTY(EditAnywhere)
		int32 CurrentSaveNumber = 0;
	UPROPERTY(EditAnywhere)
		float TotalPlayTimeInSeconds = 0.f;

	UPROPERTY(EditAnywhere)
		TArray<FSettingSavedInJsonFile> CustomSavedSettings;

	UFUNCTION(BlueprintImplementableEvent)
		void LoadSoundsVolumeFromConfig(const TArray<FSettingSavedInJsonFile> & AllSavedValuesFromConfig);

	// Get all files with wildcard from SaveGames dir, for example: "ManualSave" will give number of all files with word "ManualSave" in file name
	UFUNCTION(BlueprintImplementableEvent)
		int32 GetAllFilesWithName(const FString& WildCard);

	void SetSaveNumberAccordingToNumOfFiles();

	void SaveCustomSavedSettingsToConfig();

	void SetValueBySavedSettingName(const FString& SavedSettingName, float& Value);
	void ReplaceValueInSavedSettingByName(float NewValue, const FString& SavedSettingName);

private:

	bool bWasJsonDeserialized;
	TSharedPtr<FJsonObject> SavedDataJsonFile;
	void LoadCustomSavedSettingsFromConfig();
};
