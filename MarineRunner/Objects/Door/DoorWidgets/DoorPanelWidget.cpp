// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Door/DoorWidgets/DoorPanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Objects/Door/Door.h"
#include "PinNumberEntryObject.h"
#include "MarineRunner/Objects/Door/UsePinToEnterInterface.h"


void UDoorPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InteractDoorButton->OnClicked.AddDynamic(this, &UDoorPanelWidget::OnClickedInteractDoorButton);
	InteractDoorButton->OnHovered.AddDynamic(this, &UDoorPanelWidget::OnHoveredInteractDoorButton);
	InteractDoorButton->OnUnhovered.AddDynamic(this, &UDoorPanelWidget::OnUnhoveredInteractDoorButton);
}

#pragma region ///////////// DOOR IS UNLOCKED /////////////
void UDoorPanelWidget::OnClickedInteractDoorButton()
{
	if (ActorWithWidget == nullptr)
		return;

	ActorWithWidget->ClickedOpenButton(this);

	PlayOpenCloseEffects();
}

void UDoorPanelWidget::PlayOpenCloseEffects()
{
	if (bCanCloseObject == true)
	{
		GetWorld()->GetTimerManager().SetTimer(ChangeDoorStatusHandle, this, &UDoorPanelWidget::ChangeDoorStatus, TimeToChangeDoorStatusText, false);
	}

	PlayAnimationForward(OnClickedInteractDoorAnim);
}

void UDoorPanelWidget::ChangeDoorStatus()
{
	if (bDoorOpen == false)
	{
		bDoorOpen = true;
		InteractDoorText->SetText(CloseDoorText);
	}
	else
	{
		bDoorOpen = false;
		InteractDoorText->SetText(OpenDoorText);
	}
}

void UDoorPanelWidget::OnHoveredInteractDoorButton()
{
	if (HoverInteractDoorAnim == nullptr)
		return;

	PlayAnimationForward(HoverInteractDoorAnim);
}

void UDoorPanelWidget::OnUnhoveredInteractDoorButton()
{
	if (HoverInteractDoorAnim == nullptr)
		return;

	PlayAnimationReverse(HoverInteractDoorAnim);
}
#pragma endregion

void UDoorPanelWidget::ChangeDoorPanelToUsePin(int32 PinCode)
{
	InteractDoorButton->SetVisibility(ESlateVisibility::Hidden);
	InteractDoorText->SetVisibility(ESlateVisibility::Hidden);

	PinCodeText->SetVisibility(ESlateVisibility::Visible);
	PinNumbersTileView->SetVisibility(ESlateVisibility::Visible);
	BackgroundPinImage->SetVisibility(ESlateVisibility::Visible);

	for (const int32& CurrentPinNumberEntry : PinNumberEntries)
	{
		UPinNumberEntryObject* CreatedPinEntryObject = NewObject<UPinNumberEntryObject>(PinNumberEntryObjectClass);
		if (IsValid(CreatedPinEntryObject) == false)
			return;

		CreatedPinEntryObject->PinNumber = CurrentPinNumberEntry;
		CreatedPinEntryObject->DoorPanelWidget = this;

		PinNumbersTileView->AddItem(CreatedPinEntryObject);
	}

	CurrentPinCode = PinCode;
}

void UDoorPanelWidget::AddNumberToEnteredPin(int32 Number)
{
	CurrentlyEnteredPin += FString::FromInt(Number);
	CurrentlyEnteredPin_Text += "*";

	if (FString::FromInt(CurrentPinCode) == CurrentlyEnteredPin)
	{
		PinIsCorrect(true);
	}
	else if (CurrentlyEnteredPin.Len() == 4 && WrongCodeSound)
	{
		CurrentlyEnteredPin = "";
		CurrentlyEnteredPin_Text = "";
		UGameplayStatics::PlaySound2D(GetWorld(), WrongCodeSound);
	}

	PinCodeText->SetText(FText::FromString(CurrentlyEnteredPin_Text));
}

void UDoorPanelWidget::PinIsCorrect(bool bClickedByOwner)
{
	PinCodeText->SetVisibility(ESlateVisibility::Hidden);
	PinNumbersTileView->SetVisibility(ESlateVisibility::Hidden);
	BackgroundPinImage->SetVisibility(ESlateVisibility::Hidden);

	InteractDoorButton->SetVisibility(ESlateVisibility::Visible);
	InteractDoorText->SetVisibility(ESlateVisibility::Visible);

	if (bClickedByOwner == false)
		return;

	if (IsValid(ActorWithWidget->GetOtherPanelWidget(this)))
	{
		ActorWithWidget->GetOtherPanelWidget(this)->PinIsCorrect(false);
	}
}