// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Door/DoorWidgets/PinNumberTileEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "DoorPanelWidget.h"
#include "PinNumberEntryObject.h"


void UPinNumberTileEntry::NativeConstruct()
{
	PinNumberButton->OnClicked.AddDynamic(this, &UPinNumberTileEntry::OnClickedPinNumberButton);
	PinNumberButton->OnHovered.AddDynamic(this, &UPinNumberTileEntry::OnHoveredPinNumberButton);
	PinNumberButton->OnUnhovered.AddDynamic(this, &UPinNumberTileEntry::OnUnhoveredPinNumberButton);
}

void UPinNumberTileEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	PinNumberEntryObject = Cast<UPinNumberEntryObject>(ListItemObject);
	if (IsValid(PinNumberEntryObject) == false)
		return;

	PinNumberText->SetText(FText::AsNumber(PinNumberEntryObject->PinNumber));
}

void UPinNumberTileEntry::OnClickedPinNumberButton()
{
	if (IsValid(PinNumberEntryObject) == false)
		return;

	PinNumberEntryObject->DoorPanelWidget->AddNumberToEnteredPin(PinNumberEntryObject->PinNumber);
}

void UPinNumberTileEntry::OnHoveredPinNumberButton()
{
	if (HoverPinNumberButton == nullptr)
		return;

	PlayAnimationForward(HoverPinNumberButton);
}

void UPinNumberTileEntry::OnUnhoveredPinNumberButton()
{
	if (HoverPinNumberButton == nullptr)
		return;

	PlayAnimationReverse(HoverPinNumberButton);
}