// Copyright Adam Bartela. All Rights Reserved


#include "MarineRunner/Objects/Elevator/ElevatorPanelListEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"


#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "MarineRunner/Objects/Elevator/SelectFloorEntryObject.h"

void UElevatorPanelListEntry::NativeConstruct()
{
	SelectFloorButton->OnClicked.AddDynamic(this, &UElevatorPanelListEntry::OnClickedSelectFloorButton);
	SelectFloorButton->OnHovered.AddDynamic(this, &UElevatorPanelListEntry::OnHoveredSelectFloorButton);
	SelectFloorButton->OnUnhovered.AddDynamic(this, &UElevatorPanelListEntry::OnUnhoveredSelectFloorButton);
}

void UElevatorPanelListEntry::NativeOnInitialized()
{
}

void UElevatorPanelListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	EntryFloor = Cast<USelectFloorEntryObject>(ListItemObject);

	SetUpEntry();
}

void UElevatorPanelListEntry::SetUpEntry()
{
	if (EntryFloor->ElevatorFloor.bAccessible == true)
	{
		FString NewText = "-" + FString::FromInt(EntryFloor->ElevatorFloor.Floor) + "-";
		FloorTextBlock->SetText(FText::FromString(NewText));
	}
	else
	{
		FloorTextBlock->SetText(EntryFloor->ElevatorFloor.NotAccessibleText);
	}

	if (EntryFloor->ElevatorFloor.bStartingFloor == true)
	{
		DisableElevatorPanelEntry(true);
	}

	if (EntryFloor->ElevatorFloor.bAccessible == false)
	{
		FloorTextBlock->SetColorAndOpacity(EntryFloor->ElevatorFloor.NotAccessibleTextColor);
		SelectFloorButton->SetBackgroundColor(EntryFloor->ElevatorFloor.NotAccessibleButtonColor);
		DisableElevatorPanelEntry(true);
	}
}

void UElevatorPanelListEntry::OnClickedSelectFloorButton()
{
	if (IsValid(EntryFloor) == false)
		return;

	if (IsValid(EntryFloor->ElevatorPanelWidget) == false)
		return;

	EntryFloor->ElevatorPanelWidget->SelectFloor(EntryFloor->ElevatorFloor.Floor);

	DisablePreviousSelectedFloor();

	EntryFloor->ElevatorPanelWidget->SetCurrentSelectedFloor(EntryFloor);
	DisableElevatorPanelEntry(true);
	EntryFloor->ElevatorFloor.bStartingFloor = true;
}

void UElevatorPanelListEntry::DisablePreviousSelectedFloor()
{
	EntryFloor->ElevatorPanelWidget->GetCurrentSelectedFloor()->ElevatorFloor.bStartingFloor = false;
	UElevatorPanelListEntry* Entry = Cast<UElevatorPanelListEntry>(EntryFloor->ElevatorPanelWidget->SelectFloorsListView->GetEntryWidgetFromItem(EntryFloor->ElevatorPanelWidget->GetCurrentSelectedFloor()));
	Entry->DisableElevatorPanelEntry(false);
}

void UElevatorPanelListEntry::DisableElevatorPanelEntry(bool bDisable)
{
	SelectFloorButton->SetIsEnabled(!bDisable);
}

void UElevatorPanelListEntry::OnHoveredSelectFloorButton()
{
	if (HoverSelectFloor == nullptr)
		return;

	PlayAnimationForward(HoverSelectFloor);
}

void UElevatorPanelListEntry::OnUnhoveredSelectFloorButton()
{
	if (HoverSelectFloor == nullptr)
		return;

	PlayAnimationReverse(HoverSelectFloor);
}
