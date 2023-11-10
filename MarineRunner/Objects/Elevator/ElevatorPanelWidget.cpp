// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"

void UElevatorPanelWidget::NativeOnInitialized()
{
	ElevatorGoesDownImage->SetVisibility(ESlateVisibility::Hidden);
	ElevatorGoesUpImage->SetVisibility(ESlateVisibility::Hidden);
	WaitForElevatorImage->SetVisibility(ESlateVisibility::Hidden);

	SelectFloorsHorizontalBox->SetVisibility(ESlateVisibility::Visible);
}

void UElevatorPanelWidget::SelectFloor(int32 FloorToGo)
{
	if (IsValid(ElevatorActor) == false)
		return;

	FElevatorFloor* FoundFloor = ElevatorFloors.FindByKey(FloorToGo);
	if (FoundFloor == nullptr)
		return;

	UnhideWaitForElevatorImage();

	ElevatorActor->StartElevator(FoundFloor->FloorLocation);

	SetIsElevatorInMove(true);
}

void UElevatorPanelWidget::UnhideWaitForElevatorImage()
{
	ElevatorGoesUpImage->SetVisibility(ESlateVisibility::Hidden);
	ElevatorGoesDownImage->SetVisibility(ESlateVisibility::Hidden);

	WaitForElevatorImage->SetVisibility(ESlateVisibility::Visible);
}

void UElevatorPanelWidget::UnhideCorrectElevatorGoesUpImage(FVector FloorLocationToGo)
{
	if (IsValid(ElevatorActor) == false)
		return;

	WaitForElevatorImage->SetVisibility(ESlateVisibility::Hidden);

	if (FloorLocationToGo.Z < ElevatorActor->GetActorLocation().Z)
	{
		ElevatorGoesDownImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ElevatorGoesUpImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void UElevatorPanelWidget::SetIsElevatorInMove(bool NewIsElevatorInMove)
{
	WaitForElevatorImage->SetVisibility(ESlateVisibility::Hidden);

	SelectFloorsHorizontalBox->SetVisibility(NewIsElevatorInMove ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	SelectFloorsHorizontalBox->SetIsEnabled(!NewIsElevatorInMove);
	bIsElevatorInMove = NewIsElevatorInMove;
}