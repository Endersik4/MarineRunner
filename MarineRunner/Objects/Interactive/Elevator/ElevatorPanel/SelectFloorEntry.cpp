// Copyright Adam Bartela. All Rights Reserved


#include "SelectFloorEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"

#include "ElevatorPanelWidget.h"
#include "SelectFloorEntryObject.h"

void UElevatorPanelListEntry::NativeConstruct()
{
	SelectFloorButton->OnClicked.AddDynamic(this, &UElevatorPanelListEntry::OnClickedSelectFloorButton);
	SelectFloorButton->OnHovered.AddDynamic(this, &UElevatorPanelListEntry::OnHoveredSelectFloorButton);
	SelectFloorButton->OnUnhovered.AddDynamic(this, &UElevatorPanelListEntry::OnUnhoveredSelectFloorButton);
}

void UElevatorPanelListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	FloorEntryObject = Cast<USelectFloorEntryObject>(ListItemObject);

	SetUpElevatorEntry();
}

void UElevatorPanelListEntry::SetUpElevatorEntry()
{
	if (!IsValid(FloorEntryObject))
		return;

	if (FloorEntryObject->ElevatorFloor.bAccessible)
	{
		FloorIsAccessible();
	}
	else
	{
		FloorTextBlock->SetText(FloorEntryObject->ElevatorFloor.NotAccessibleText);
		FloorTextBlock->SetColorAndOpacity(FloorEntryObject->ElevatorFloor.NotAccessibleTextColor);
		SelectFloorButton->SetBackgroundColor(FloorEntryObject->ElevatorFloor.NotAccessibleButtonColor);
		DisableElevatorPanelEntry(true);
	}
}

void UElevatorPanelListEntry::FloorIsAccessible()
{
	FString NewFloorText = "-" + FString::FromInt(FloorEntryObject->ElevatorFloor.Floor) + "-";
	if (FloorEntryObject->ElevatorFloor.Floor == FloorEntryObject->ElevatorPanelWidget->GetCurrentFloor())
	{
		NewFloorText = "-curr-";
	}
	FloorTextBlock->SetText(FText::FromString(NewFloorText));

	if (FloorEntryObject->ElevatorFloor.Floor == FloorEntryObject->ElevatorPanelWidget->GetCurrentFloor() && FloorEntryObject->ElevatorPanelWidget->GetDoorOpen())
	{
		DisableElevatorPanelEntry(true);
	}
	else
	{
		DisableElevatorPanelEntry(false);
	}

	FloorTextBlock->SetColorAndOpacity(FloorEntryObject->ElevatorFloor.AccessibleTextColor);
	SelectFloorButton->SetBackgroundColor(FloorEntryObject->ElevatorFloor.AccessibleButtonColor);
}

void UElevatorPanelListEntry::OnClickedSelectFloorButton()
{
	if (!IsValid(FloorEntryObject))
		return;

	if (!IsValid(FloorEntryObject->ElevatorPanelWidget))
		return;

	FloorEntryObject->ElevatorPanelWidget->SelectFloor(FloorEntryObject->ElevatorFloor.Floor);
}

void UElevatorPanelListEntry::DisableElevatorPanelEntry(bool bDisable)
{
	SelectFloorButton->SetIsEnabled(!bDisable);
}

void UElevatorPanelListEntry::OnHoveredSelectFloorButton()
{
	if (!HoverSelectFloor)
		return;

	PlayAnimationForward(HoverSelectFloor);
}

void UElevatorPanelListEntry::OnUnhoveredSelectFloorButton()
{
	if (!HoverSelectFloor)
		return;

	PlayAnimationReverse(HoverSelectFloor);
}