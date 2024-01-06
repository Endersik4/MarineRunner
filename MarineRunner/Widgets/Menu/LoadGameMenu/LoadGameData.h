// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadGameData.generated.h"


/**
 * 
 */

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
	}

	FSaveDataMenuStruct(FString NewName, int32 NewSaveNumber, FString NewScreenshotPath, FString NewSaveDate, FString NewLevelName, float NewTotalTimeInMinutes)
	{
		SaveName = NewName;
		SaveNumber = NewSaveNumber;
		ScreenshotPathSave = NewScreenshotPath;
		SaveDateTime = NewSaveDate;
		LevelNameToLoad = NewLevelName;
		TotalPlayTimeInSeconds = NewTotalTimeInMinutes;
	}

	bool operator<(const FSaveDataMenuStruct& OtherSaveDataMenu)
	{
		return SaveNumber < OtherSaveDataMenu.SaveNumber;
	}
};

class MARINERUNNER_API LoadGameData
{
};
