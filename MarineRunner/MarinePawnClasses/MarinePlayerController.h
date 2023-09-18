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
	virtual void SetupInputComponent() override;

public:
	inline void SetMouseSensitivity(float NewMouseSens) { MouseSensitivity = NewMouseSens; }
	inline float GetMouseSensitivity() { return MouseSensitivity; }
		
	float MouseXValue;
	float MouseYValue;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Marine Player Controller Settings")
		FFloatRange MarginForPlayerToMove = FFloatRange(-0.75f, 0.75f);

	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	float MouseSensitivity = 0.7f;

	bool bShouldDisableCameraRotate(float CurrentLookValue);
	// if player moves too much mouse while camera is rotating (for wallrun) then disable camera rotating
	void DisableCameraRotateWhileWallrunning(float CurrentLookValue);

	class AMarineCharacter* MarinePawn;
};
