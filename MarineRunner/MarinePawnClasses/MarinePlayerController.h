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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MouseXValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MouseYValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MouseSens = 0.7f;
};
