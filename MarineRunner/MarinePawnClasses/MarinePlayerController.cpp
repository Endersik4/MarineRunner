// Copyright Adam Bartela.All Rights Reserved

#include "MarinePlayerController.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"

void AMarinePlayerController::BeginPlay()
{
	Super::BeginPlay();
	MarinePawn = Cast<AMarineCharacter>(GetPawn());

	OnPossessedPawnChanged.AddDynamic(this, &AMarinePlayerController::HandleOnPossessedPawnChanged);
}

void AMarinePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsValid(InputComponent) == false)
		return;

	InputComponent->BindAxis(TEXT("LookUp"), this, &AMarinePlayerController::LookUp);
	InputComponent->BindAxis(TEXT("LookRight"), this, &AMarinePlayerController::LookRight);
}

void AMarinePlayerController::HandleOnPossessedPawnChanged(APawn* _NewPawn, APawn* _OldPawn)
{
	MarinePawn = Cast<AMarineCharacter>(_NewPawn);
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
	{
		return false;
	}

	if (MarinePawn->GetWallrunComponent()->GetIsWallrunning() == false)
	{
		return false;
	}

	if (CurrentLookValue > MarginForPlayerToMove.GetLowerBoundValue() && CurrentLookValue < MarginForPlayerToMove.GetUpperBoundValue())
	{
		return false;
	}
	return true;
}

void AMarinePlayerController::DisableCameraRotateWhileWallrunning(float CurrentLookValue)
{
	if (bShouldDisableCameraRotate(CurrentLookValue) == false)
		return;

	MarinePawn->GetWallrunComponent()->SetShouldLerpRotation(false);
}

