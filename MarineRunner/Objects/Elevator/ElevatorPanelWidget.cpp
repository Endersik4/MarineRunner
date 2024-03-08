// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ListView.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "MarineRunner/Objects/Elevator/SelectFloorEntryObject.h"

void UElevatorPanelWidget::NativeOnInitialized()
{
	ElevatorGoesUpDownImage->SetVisibility(ESlateVisibility::Hidden);
	WaitForElevatorTextBlock->SetVisibility(ESlateVisibility::Hidden);

	SelectFloorsListView->SetVisibility(ESlateVisibility::Visible);
}

void UElevatorPanelWidget::FillSelectFloorsListView()
{
	SelectFloorsListView->ClearListItems();

	for (const FElevatorFloor& SelectedFloor : AllElevatorFloors)
	{
		TObjectPtr<USelectFloorEntryObject> ConstructedFloorEntryObject = NewObject<USelectFloorEntryObject>(SelectedFloorEntryObject);
		if (!IsValid(ConstructedFloorEntryObject))
			continue;

		ConstructedFloorEntryObject->ElevatorFloor = SelectedFloor;
		ConstructedFloorEntryObject->ElevatorPanelWidget = this;

		SelectFloorsListView->AddItem(ConstructedFloorEntryObject);
	}
}

void UElevatorPanelWidget::SelectFloor(int32 FloorToGo)
{
	if (!IsValid(ElevatorActor))
		return;

	FElevatorFloor* FoundFloor = AllElevatorFloors.FindByKey(FloorToGo);
	if (!FoundFloor)
		return;

	ElevatorActor->PrepareElevatorToMove(FoundFloor->FloorLocation, FoundFloor->Floor);

	ShowSelectFloorPanel(false);
}

/// <summary>
/// Show WaitForElevator Text by playing animations
/// </summary>
/// <param name="bShow">if true then play WaitTextBlockAppearAnim, play WaitTextBlockDisappearAnim and stop WaitTextBlockActiveAnim otherwise</param>
void UElevatorPanelWidget::ShowWaitForElevatorText(bool bShow)
{
	if (bShow && WaitTextBlockActiveAnim)
	{
		PlayAnimation(WaitTextBlockAppearAnim);
	}
	else if (WaitTextBlockDisappearAnim)
	{
		StopAnimation(WaitTextBlockActiveAnim);
		PlayAnimation(WaitTextBlockDisappearAnim);
	}
}

/// <summary>
/// Show Elevator Goes Up/Down Image by playing animation ElevatorGoesUpDownAppearAnim/ElevatorGoesUpDownDisappearAnim 
/// and hide WaitEffect. Sets if this should be Up or Down 
/// </summary>
/// <param name="bShow"> if true then show Up/Down Image, hide otherwise </param>
/// <param name="FloorLocationToGo">Compares the Z location of the floor with the current Z location of the ElevatorActor and if it is smaller, down, otherwise up</param>
void UElevatorPanelWidget::ShowElevatorGoesUpDownImage(bool bShow, FVector FloorLocationToGo)
{
	if (!IsValid(ElevatorActor))
		return;

	// Hide WaitEffect if Elevator Goes Up/Down
	if (bShow)
	{
		float& RotateImageAngle = FloorLocationToGo.Z < ElevatorActor->GetActorLocation().Z ? ElevatorGoesDownImageAngle : ElevatorGoesUpImageAngle;
		ElevatorGoesUpDownImage->SetRenderTransformAngle(RotateImageAngle);
	}

	if (bShow && ElevatorGoesUpDownAppearAnim && ElevatorGoesUpDownImage->GetVisibility() == ESlateVisibility::Hidden)
	{
		PlayAnimation(ElevatorGoesUpDownAppearAnim);
	}
	else if (ElevatorGoesUpDownDisappearAnim && ElevatorGoesUpDownImage->GetVisibility() == ESlateVisibility::Visible)
	{
		PlayAnimation(ElevatorGoesUpDownDisappearAnim);
	}
}

void UElevatorPanelWidget::ShowSelectFloorPanel(bool bActivate)
{
	SelectFloorsListView->SetIsEnabled(bActivate);
	if (bActivate)
	{
		FillSelectFloorsListView();

		if (SelectFloorsAppearAnim)
			PlayAnimation(SelectFloorsAppearAnim);
	}
	else if (SelectFloorsDisappearAnim)
	{
		PlayAnimation(SelectFloorsDisappearAnim);
	}
}

int32 UElevatorPanelWidget::GetCurrentFloor() const
{
	return ElevatorActor->GetCurrentFloor();
}

bool UElevatorPanelWidget::GetDoorOpen() const
{
	return ElevatorActor->GetDoorOpen();

}