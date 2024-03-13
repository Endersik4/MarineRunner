// Copyright Adam Bartela.All Rights Reserved


#include "PinNumberTileEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "PanelWithPinWidget.h"
#include "PinNumberEntryObject.h"
#include "MarineRunner/Objects/Interactive/Elevator/OutsideElevatorDoors/CallElevatorPanelWidget.h"


void UPinNumberTileEntry::NativeConstruct()
{
	PinNumberButton->OnClicked.AddDynamic(this, &UPinNumberTileEntry::OnClickedPinNumberButton);
	PinNumberButton->OnHovered.AddDynamic(this, &UPinNumberTileEntry::OnHoveredPinNumberButton);
	PinNumberButton->OnUnhovered.AddDynamic(this, &UPinNumberTileEntry::OnUnhoveredPinNumberButton);
}

void UPinNumberTileEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	PinNumberEntryObject = Cast<UPinNumberEntryObject>(ListItemObject);
	if (!IsValid(PinNumberEntryObject))
		return;

	PinNumberText->SetText(FText::AsNumber(PinNumberEntryObject->PinNumber));
}

void UPinNumberTileEntry::OnClickedPinNumberButton()
{
	if (!IsValid(PinNumberEntryObject))
		return;

	if (IsValid(PinNumberEntryObject->DoorPanelWidget))
		PinNumberEntryObject->DoorPanelWidget->AddNumberToEnteredPin(PinNumberEntryObject->PinNumber);
	if (IsValid(PinNumberEntryObject->CallElevatorPanelWidget))
		PinNumberEntryObject->CallElevatorPanelWidget->AddNumberToEnteredPin(PinNumberEntryObject->PinNumber);
}

void UPinNumberTileEntry::OnHoveredPinNumberButton()
{
	if (!HoverPinNumberButton)
		return;

	PlayAnimationForward(HoverPinNumberButton);
}

void UPinNumberTileEntry::OnUnhoveredPinNumberButton()
{
	if (!HoverPinNumberButton)
		return;

	PlayAnimationReverse(HoverPinNumberButton);
}
