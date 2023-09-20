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
		int32 TotalPlayTimeInMinutes;

	FSaveDataMenuStruct()
	{
		SaveName = "";
		ScreenshotPathSave = "";
		SaveDateTime = "";
		TotalPlayTimeInMinutes = 0;
	}

	FSaveDataMenuStruct(FString NewName, FString NewScreenshotPath, FString NewSaveDate, int32 NewTotalTimeInMinutes)
	{
		SaveName = NewName;
		ScreenshotPathSave = NewScreenshotPath;
		SaveDateTime = NewSaveDate;
		TotalPlayTimeInMinutes = NewTotalTimeInMinutes;
	}
};

class MARINERUNNER_API LoadGameData
{
};
