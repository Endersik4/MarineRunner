// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ListView.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "MarineRunner/Objects/Elevator/SelectFloorEntryObject.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelListEntry.h"


void UElevatorPanelWidget::NativeOnInitialized()
{
	ElevatorGoesUpDownImage->SetVisibility(ESlateVisibility::Hidden);
	WaitForElevatorTextBlock->SetVisibility(ESlateVisibility::Hidden);

	SelectFloorsListView->SetVisibility(ESlateVisibility::Visible);

	FillSelectFloorsListView();
}

void UElevatorPanelWidget::FillSelectFloorsListView()
{
	for (const FElevatorFloor& SelectedFloor : ElevatorFloors)
	{
		USelectFloorEntryObject* ConstructedItemObject = NewObject<USelectFloorEntryObject>(SelectedFloorEntryObject);
		if (IsValid(ConstructedItemObject) == false)
			continue;

		ConstructedItemObject->ElevatorFloor = SelectedFloor;
		ConstructedItemObject->ElevatorPanelWidget = this;

		if (SelectedFloor.bStartingFloor == true)
			CurrentSelectedFloor = ConstructedItemObject;

		SelectFloorsListView->AddItem(ConstructedItemObject);
	}
}

void UElevatorPanelWidget::SelectFloor(int32 FloorToGo)
{
	if (IsValid(ElevatorActor) == false)
		return;

	FElevatorFloor* FoundFloor = ElevatorFloors.FindByKey(FloorToGo);
	if (FoundFloor == nullptr)
		return;

	DisablePreviousSelectedFloor();
	EnableCurrentSelectedFloor(FloorToGo);

	ElevatorActor->StartElevator(FoundFloor->FloorLocation, FoundFloor->Floor);

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

	if (bActivate) SelectFloorsListView->SetVisibility(ESlateVisibility::Visible);
	SelectFloorsListView->SetIsEnabled(bActivate);
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

void UElevatorPanelWidget::DisablePreviousSelectedFloor()
{
	if (IsValid(CurrentSelectedFloor) == false)
		return;

	CurrentSelectedFloor->ElevatorFloor.bStartingFloor = false;
	UElevatorPanelListEntry* Entry = Cast<UElevatorPanelListEntry>(SelectFloorsListView->GetEntryWidgetFromItem(CurrentSelectedFloor));
	if (Entry)
	{
		Entry->DisableElevatorPanelEntry(false);
	}
}

void UElevatorPanelWidget::EnableCurrentSelectedFloor(int32 CurrentFloor)
{
	for (UObject* CurrentFloorEntryObject : SelectFloorsListView->GetListItems())
	{
		USelectFloorEntryObject* ConstructedItemObject = Cast<USelectFloorEntryObject>(CurrentFloorEntryObject);
		if (IsValid(ConstructedItemObject) == false)
			return;

		if (*ConstructedItemObject != CurrentFloor)
			continue;
	
		CurrentSelectedFloor = ConstructedItemObject;
		ConstructedItemObject->ElevatorFloor.bStartingFloor = true;
		UElevatorPanelListEntry* Entry = Cast<UElevatorPanelListEntry>(SelectFloorsListView->GetEntryWidgetFromItem(ConstructedItemObject));
		if (Entry)
		{
			Entry->DisableElevatorPanelEntry(true);
		}
	}
}
