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
		ScreenshotPathSave = "";
		SaveDateTime = "";
		TotalPlayTimeInSeconds = 0.f;
	}

	FSaveDataMenuStruct(FString NewName, FString NewScreenshotPath, FString NewSaveDate, FString NewLevelName, float NewTotalTimeInMinutes)
	{
		SaveName = NewName;
		ScreenshotPathSave = NewScreenshotPath;
		SaveDateTime = NewSaveDate;
		LevelNameToLoad = NewLevelName;
		TotalPlayTimeInSeconds = NewTotalTimeInMinutes;
	}
};

class MARINERUNNER_API LoadGameData
{
};
