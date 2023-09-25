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


public:
	UPROPERTY(EditAnywhere)
		FString SlotSaveGameNameToLoad = "MySlot";
	UPROPERTY(EditAnywhere)
		int32 CurrentSaveNumber = 0;
	UPROPERTY(EditAnywhere)
		float TotalPlayTimeInSeconds = 0.f;
};
