// Fill out your copyright notice in the Description page of Project Settings.


#include "MarinePlayerController.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"

void AMarinePlayerController::BeginPlay()
{
	Super::BeginPlay();
	MarinePawn = Cast<AMarineCharacter>(GetPawn());
}

void AMarinePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsValid(InputComponent) == false)
		return;

	InputComponent->BindAxis(TEXT("LookUp"), this, &AMarinePlayerController::LookUp);
	InputComponent->BindAxis(TEXT("LookRight"), this, &AMarinePlayerController::LookRight);
}

void AMarinePlayerController::LookUp(float AxisValue)
{
	AddPitchInput(MouseSensitivity.FieldValue * AxisValue);
	DisableCameraRotateWhileWallrunning(AxisValue);
	MouseXValue = AxisValue;
}

void AMarinePlayerController::LookRight(float AxisValue)
{
	AddYawInput(MouseSensitivity.FieldValue * AxisValue);
	DisableCameraRotateWhileWallrunning(AxisValue);
	MouseYValue = AxisValue;
}

bool AMarinePlayerController::bShouldDisableCameraRotate(float CurrentLookValue)
{
	if (IsValid(MarinePawn) == false)
		return false;

	if (MarinePawn->GetWallrunComponent()->GetIsWallrunning() == false)
		return false;

	if (CurrentLookValue < MarginForPlayerToMove.GetUpperBoundValue() && CurrentLookValue > MarginForPlayerToMove.GetLowerBoundValue())
		return false;

	return true;
}

void AMarinePlayerController::DisableCameraRotateWhileWallrunning(float CurrentLookValue)
{
	if (bShouldDisableCameraRotate(CurrentLookValue) == false)
		return;

	MarinePawn->GetWallrunComponent()->SetShouldLerpRotation(false);
}

