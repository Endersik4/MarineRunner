// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"

void UElevatorPanelWidget::NativeOnInitialized()
{
	ElevatorGoesUpDownImage->SetVisibility(ESlateVisibility::Hidden);
	WaitForElevatorTextBlock->SetVisibility(ESlateVisibility::Hidden);

	SelectFloorsHorizontalBox->SetVisibility(ESlateVisibility::Visible);
}

void UElevatorPanelWidget::SelectFloor(int32 FloorToGo)
{
	if (IsValid(ElevatorActor) == false)
		return;

	FElevatorFloor* FoundFloor = ElevatorFloors.FindByKey(FloorToGo);
	if (FoundFloor == nullptr)
		return;

	ElevatorActor->StartElevator(FoundFloor->FloorLocation);

	ActiveSelectFloorPanel(false);
}

void UElevatorPanelWidget::ActivateWaitForElevatorText(bool bActivate)
{
	if (bActivate) WaitForElevatorTextBlock->SetVisibility(ESlateVisibility::Visible);

	if (bActivate && WaitTextBlockActiveAnim)
	{
		PlayAnimation(WaitTextBlockAppearAnim);
	}
	else if (WaitTextBlockDisappearAnim)
	{
		StopAnimation(WaitTextBlockActiveAnim);
		PlayAnimation(WaitTextBlockDisappearAnim);
	}
}

void UElevatorPanelWidget::ActivateElevatorGoesUpDownImage(bool bActivate, FVector FloorLocationToGo)
{
	if (IsValid(ElevatorActor) == false)
		return;

	if (bActivate == true)
	{
		ActivateWaitForElevatorText(false);
		ElevatorGoesUpDownImage->SetVisibility(ESlateVisibility::Visible);
	}

	if (bActivate && ElevatorGoesUpDownAppearAnim)
	{
		PlayAnimation(ElevatorGoesUpDownAppearAnim);
	}
	else if (ElevatorGoesUpDownDisappearAnim)
	{
		PlayAnimation(ElevatorGoesUpDownDisappearAnim);
	}

	if (bActivate == false)
	{
		return;
	}

	if (FloorLocationToGo.Z < ElevatorActor->GetActorLocation().Z)
	{
		ElevatorGoesUpDownImage->SetRenderTransformAngle(ElevatorGoesDownImageAngle);
	}
	else
	{
		ElevatorGoesUpDownImage->SetRenderTransformAngle(ElevatorGoesUpImageAngle);
	}
}

void UElevatorPanelWidget::ActiveSelectFloorPanel(bool bActivate)
{
	ActivateWaitForElevatorText(!bActivate);

	if (bActivate) SelectFloorsHorizontalBox->SetVisibility(ESlateVisibility::Visible);
	SelectFloorsHorizontalBox->SetIsEnabled(bActivate);
	bIsElevatorInMove = !bActivate;

	if (bActivate && SelectFloorsAppearAnim)
	{
		PlayAnimation(SelectFloorsAppearAnim);
	}
	else if (SelectFloorsDisappearAnim)
	{
		PlayAnimation(SelectFloorsDisappearAnim);
	}
}