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

	if (!IsValid(InputComponent))
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

void AMarinePlayerController::DisableCameraRotateWhileWallrunning(float CurrentLookValue)
{
	if (ShouldDisableCameraWallrunRotate(CurrentLookValue) == false)
		return;

	MarinePawn->GetWallrunComponent()->SetShouldCameraYawRotate(false);
}

bool AMarinePlayerController::ShouldDisableCameraWallrunRotate(float CurrentLookValue)
{
	if (!IsValid(MarinePawn))
		return false;

	if (!MarinePawn->GetWallrunComponent()->GetIsWallrunning())
		return false;

	if (CurrentLookValue > MarginForPlayerToMove.GetLowerBoundValue() && CurrentLookValue < MarginForPlayerToMove.GetUpperBoundValue())
	{
		return false;
	}
	return true;
}
