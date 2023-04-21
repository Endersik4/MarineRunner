// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MarinePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API AMarinePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MouseXValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MouseYValue;
};
