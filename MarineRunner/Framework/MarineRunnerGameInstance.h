// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MarineRunnerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UMarineRunnerGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
		virtual void Init() override;
public:
	UPROPERTY(EditAnywhere)
		FString SlotSaveGameNameToLoad = "MySlot";
	UPROPERTY(EditAnywhere)
		int32 CurrentSaveNumber = 0;
	UPROPERTY(EditAnywhere)
		float TotalPlayTimeInSeconds = 0.f;

	// Get all files with wildcard from SaveGames dir, for example: "ManualSave" will give number of all files with word "ManualSave" in file name
	UFUNCTION(BlueprintImplementableEvent)
		int32 GetAllFilesWithName(const FString& WildCard);

	void SetSaveNumberAccordingToNumOfFiles();
};
