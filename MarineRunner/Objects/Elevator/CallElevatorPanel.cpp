// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/CallElevatorPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/TileView.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "MarineRunner/Objects/Elevator/OutsideElevatorDoor.h"
#include "MarineRunner/Objects/Door/UsePinToEnterInterface.h"
#include "MarineRunner/Objects/Door/DoorWidgets/PinNumberEntryObject.h"


void UCallElevatorPanel::NativeOnInitialized()
{
	CallElevatorButton->OnClicked.AddDynamic(this, &UCallElevatorPanel::OnClickedCallElevatorButton);
	CallElevatorButton->OnHovered.AddDynamic(this, &UCallElevatorPanel::OnHoveredCallElevatorButton);
	CallElevatorButton->OnUnhovered.AddDynamic(this, &UCallElevatorPanel::OnUnhoveredCallElevatorButton);

	WaitForElevatorText->SetVisibility(ESlateVisibility::Hidden);
}

void UCallElevatorPanel::OnClickedCallElevatorButton()
{
	if (IsValid(ElevatorActor) == false)
		return;

	ElevatorActor->GetElevatorPanelWidget()->SelectFloor(Floor);

	CallElevatorAction(ECEA_HideCallAndShowWait);
}

void UCallElevatorPanel::OnHoveredCallElevatorButton()
{
	if (HoverCallElevatorAnim == nullptr)
		return;

	PlayAnimationForward(HoverCallElevatorAnim);
}

void UCallElevatorPanel::OnUnhoveredCallElevatorButton()
{
	if (HoverCallElevatorAnim == nullptr)
		return;

	PlayAnimationReverse(HoverCallElevatorAnim);
}

void UCallElevatorPanel::ShowWaitText(bool bShow)
{
	if (bShow)
	{
		if (AppearWaitForElevatorAnim)
			PlayAnimationForward(AppearWaitForElevatorAnim);
	}
	else
	{
		if (DisappearWaitForElevatorAnim)
			PlayAnimationForward(DisappearWaitForElevatorAnim);
	}
}

void UCallElevatorPanel::ShowCallElevatorPanel(bool bShow)
{
	if (bShow)
	{
		if (AppearCallElevatorAnim)
			PlayAnimationForward(AppearCallElevatorAnim);
	}
	else
	{
		if (DisappearCallElevatorAnim)
			PlayAnimationForward(DisappearCallElevatorAnim);
	}
}

void UCallElevatorPanel::CallElevatorAction(const ECallElevatorAction ActionToDo)
{
	switch (ActionToDo)
	{
		case ECallElevatorAction::ECEA_ShowCall:
		{
			ShowCallElevatorPanel(true);
		}break;
		case ECallElevatorAction::ECEA_HideCallAndShowWait: 
		{
			ShowCallElevatorPanel(false);
			ShowWaitText(true);
		} break;
		case ECallElevatorAction::ECEA_HideCall:
		{
			ShowCallElevatorPanel(false);
		} break;
		case ECallElevatorAction::ECEA_ShowWaitEffect:
		{
			ShowWaitText(true);
		}break;
		case ECallElevatorAction::ECEA_HideWaitEffect:
		{
			ShowWaitText(false);
		}break;
		default: break;
	}
}

bool UCallElevatorPanel::CanCallElevator() const
{
	return CallElevatorButton->GetIsEnabled();
}

void UCallElevatorPanel::AddNumberToEnteredPin(int32 Number)
{
	CurrentlyEnteredPin += FString::FromInt(Number);
	CurrentlyEnteredPin_Text += "*";

	if (FString::FromInt(CurrentPinCode) == CurrentlyEnteredPin)
	{
		PinIsCorrect();
		if (IsValid(OutsideElevatorActor) == false)
			return;
		OutsideElevatorActor->PinIsCorrect();
	}
	else if (CurrentlyEnteredPin.Len() == 4 && WrongCodeSound)
	{
		CurrentlyEnteredPin = "";
		CurrentlyEnteredPin_Text = "";
		UGameplayStatics::PlaySound2D(GetWorld(), WrongCodeSound);
	}

	PinCodeText->SetText(FText::FromString(CurrentlyEnteredPin_Text));
}

void UCallElevatorPanel::ChangeDoorPanelToUsePin(int32 PinCode)
{
	CallElevatorButton->SetVisibility(ESlateVisibility::Hidden);
	CallElevatorText->SetVisibility(ESlateVisibility::Hidden);
	WaitForElevatorText->SetVisibility(ESlateVisibility::Hidden);
	
	MaintanceModeTextBlock->SetText(LockedByPinModeText);
	MaintanceModeTextBlock->SetColorAndOpacity(LockedByPinColorText);

	PinCodeText->SetVisibility(ESlateVisibility::Visible);
	PinNumbersTileView->SetVisibility(ESlateVisibility::Visible);
	BackgroundPinImage->SetVisibility(ESlateVisibility::Visible);

	for (const int32& CurrentPinNumberEntry : PinNumberEntries)
	{
		UPinNumberEntryObject* CreatedPinEntryObject = NewObject<UPinNumberEntryObject>(PinNumberEntryObjectClass);
		if (IsValid(CreatedPinEntryObject) == false)
			return;

		CreatedPinEntryObject->PinNumber = CurrentPinNumberEntry;
		CreatedPinEntryObject->CallElevatorPanelWidget = this;

		PinNumbersTileView->AddItem(CreatedPinEntryObject);
	}

	CurrentPinCode = PinCode;
}

void UCallElevatorPanel::PinIsCorrect()
{
	PinCodeText->SetVisibility(ESlateVisibility::Hidden);
	PinNumbersTileView->SetVisibility(ESlateVisibility::Hidden);
	BackgroundPinImage->SetVisibility(ESlateVisibility::Hidden);

	CallElevatorButton->SetVisibility(ESlateVisibility::Visible);
	CallElevatorText->SetVisibility(ESlateVisibility::Visible);
	WaitForElevatorText->SetVisibility(ESlateVisibility::Hidden);

	MaintanceModeTextBlock->SetText(UnlockedByPinModeText);
	MaintanceModeTextBlock->SetColorAndOpacity(UnlockedByPinColorText);
}