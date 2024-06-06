// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "LoadSaveInformationData.generated.h"

USTRUCT(BlueprintType)
struct FSaveDataMenuStruct
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
	FString SaveName;
	UPROPERTY(EditDefaultsOnly)
	int32 SaveNumber = 1;
	UPROPERTY(EditDefaultsOnly)
	FString ScreenshotPathSave;
	UPROPERTY(EditDefaultsOnly)
	FString SaveDateTime;
	UPROPERTY(EditDefaultsOnly)
	int32 SavedDateValue;
	UPROPERTY(EditDefaultsOnly)
	FString LevelNameToLoad;
	UPROPERTY(EditDefaultsOnly)
	float TotalPlayTimeInSeconds;

	FSaveDataMenuStruct()
	{
		SaveName = "";
		SaveNumber = 1;
		ScreenshotPathSave = "";
		SaveDateTime = "";
		TotalPlayTimeInSeconds = 0.f;
		SavedDateValue = 0;
	}

	FSaveDataMenuStruct(FString NewName, int32 NewSaveNumber, FString NewScreenshotPath, FString NewSaveDate, FString NewLevelName, float NewTotalTimeInMinutes, int32 NewSavedDateValue)
	{
		SaveName = NewName;
		SaveNumber = NewSaveNumber;
		ScreenshotPathSave = NewScreenshotPath;
		SaveDateTime = NewSaveDate;
		LevelNameToLoad = NewLevelName;
		TotalPlayTimeInSeconds = NewTotalTimeInMinutes;
		SavedDateValue = NewSavedDateValue;
	}

	FORCEINLINE bool operator<(const FSaveDataMenuStruct& OtherSaveDataMenu) const
	{
		return SaveNumber < OtherSaveDataMenu.SaveNumber;
	}
};

class MARINERUNNER_API LoadSaveInformationData
{

};
