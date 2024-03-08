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
	if (!IsValid(ElevatorActor))
		return;

	ElevatorActor->GetElevatorPanelWidget()->SelectFloor(FloorToMove);

	CallElevatorAction(ECEA_HideCallAndShowWait);
}

void UCallElevatorPanel::OnHoveredCallElevatorButton()
{
	if (!HoverCallElevatorAnim)
		return;

	PlayAnimationForward(HoverCallElevatorAnim);
}

void UCallElevatorPanel::OnUnhoveredCallElevatorButton()
{
	if (!HoverCallElevatorAnim)
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

	// if pin is correct then hide pin panel
	if (FString::FromInt(CurrentPinCode) == CurrentlyEnteredPin)
	{
		PinIsCorrect();
		if (IsValid(OutsideElevatorActor))
			OutsideElevatorActor->PinIsCorrect();
	}
	else if (CurrentlyEnteredPin.Len() == MaxPinCodeLength) // if not then reset pin play wrong code sound 
	{
		CurrentlyEnteredPin = "";
		CurrentlyEnteredPin_Text = "";
		if (IsValid(WrongCodeSound))
			UGameplayStatics::PlaySound2D(GetWorld(), WrongCodeSound);
	}

	PinCodeText->SetText(FText::FromString(CurrentlyEnteredPin_Text));
}

void UCallElevatorPanel::ChangeToUsePin(int32 PinCode)
{
	PinNumbersTileView->ClearListItems();

	HidePin(false);

	for (const int32& CurrentPinNumberEntry : PinNumberEntries)
	{
		TObjectPtr<UPinNumberEntryObject> CreatedPinEntryObject = NewObject<UPinNumberEntryObject>(PinNumberEntryObjectClass);
		if (!IsValid(CreatedPinEntryObject))
			continue;

		CreatedPinEntryObject->PinNumber = CurrentPinNumberEntry;
		CreatedPinEntryObject->CallElevatorPanelWidget = this;

		PinNumbersTileView->AddItem(CreatedPinEntryObject);
	}

	CurrentlyEnteredPin.Empty();
	CurrentlyEnteredPin_Text.Empty();
	PinCodeText->SetText(FText::FromString(CurrentlyEnteredPin_Text));
	CurrentPinCode = PinCode;
}

void UCallElevatorPanel::PinIsCorrect()
{
	HidePin(true);
}

void UCallElevatorPanel::HidePin(bool bHide)
{
	PinCodeText->SetVisibility(bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	PinNumbersTileView->SetVisibility(bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	BackgroundPinImage->SetVisibility(bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

	CallElevatorButton->SetVisibility(bHide ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	CallElevatorText->SetVisibility(bHide ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	WaitForElevatorText->SetVisibility(ESlateVisibility::Hidden);

	MaintanceModeTextBlock->SetText(bHide ? UnlockedByPinModeText : LockedByPinModeText);
	MaintanceModeTextBlock->SetColorAndOpacity(bHide ? UnlockedByPinColorText : LockedByPinColorText);
}
