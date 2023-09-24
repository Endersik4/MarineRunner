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
};
