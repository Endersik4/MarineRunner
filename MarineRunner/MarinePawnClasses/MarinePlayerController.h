// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

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
	FORCEINLINE void SetMouseSensitivity(const FSettingSavedInJsonFile &NewMouseSensitivity) { MouseSensitivity = NewMouseSensitivity; }
	FORCEINLINE const FSettingSavedInJsonFile &GetMouseSensitivity() const { return MouseSensitivity; }

	float MouseXValue;
	float MouseYValue;

	UFUNCTION()
		void HandleOnPossessedPawnChanged(APawn* _NewPawn, APawn* _OldPawn);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Marine Player Controller Settings")
		FFloatRange MarginForPlayerToMove = FFloatRange(-0.2f, 0.2f);

	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	FSettingSavedInJsonFile MouseSensitivity = FSettingSavedInJsonFile("MouseSensitivity", 0.7f);

	bool bShouldDisableCameraRotate(float CurrentLookValue);
	// if player moves too much mouse while camera is rotating (for wallrun) then disable camera rotating
	void DisableCameraRotateWhileWallrunning(float CurrentLookValue);

	class AMarineCharacter* MarinePawn;
};
