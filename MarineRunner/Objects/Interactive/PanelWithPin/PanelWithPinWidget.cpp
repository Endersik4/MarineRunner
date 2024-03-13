// Copyright Adam Bartela.All Rights Reserved


#include "PanelWithPinWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"

#include "PinNumberEntryObject.h"
#include "UsePinToEnterInterface.h"


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
	if (!ActorWithWidget)
		return;

	ActorWithWidget->ClickedOpenButton(this);

	PlayOpenCloseEffects();
}

void UDoorPanelWidget::PlayOpenCloseEffects()
{
	if (bCanCloseObject)
		GetWorld()->GetTimerManager().SetTimer(ChangeDoorStatusHandle, this, &UDoorPanelWidget::ChangeDoorStatus, TimeToChangeStatusText, false);

	PlayAnimationForward(OnClickedInteractDoorAnim);
}

void UDoorPanelWidget::ChangeDoorStatus()
{
	if (!bDoorOpen)
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
	if (!HoverInteractDoorAnim)
		return;

	PlayAnimationForward(HoverInteractDoorAnim);
}

void UDoorPanelWidget::OnUnhoveredInteractDoorButton()
{
	if (!HoverInteractDoorAnim)
		return;

	PlayAnimationReverse(HoverInteractDoorAnim);
}
#pragma endregion

void UDoorPanelWidget::ChangeToUsePin(int32 PinCode)
{
	PinNumbersTileView->ClearListItems();

	InteractDoorButton->SetVisibility(ESlateVisibility::Hidden);
	InteractDoorText->SetVisibility(ESlateVisibility::Hidden);

	PinCodeText->SetVisibility(ESlateVisibility::Visible);
	PinNumbersTileView->SetVisibility(ESlateVisibility::Visible);
	BackgroundPinImage->SetVisibility(ESlateVisibility::Visible);

	for (const int32& CurrentPinNumberEntry : PinNumberEntries)
	{
		TObjectPtr<UPinNumberEntryObject> CreatedPinEntryObject = NewObject<UPinNumberEntryObject>(PinNumberEntryObjectClass);
		if (!IsValid(CreatedPinEntryObject))
			return;

		CreatedPinEntryObject->PinNumber = CurrentPinNumberEntry;
		CreatedPinEntryObject->DoorPanelWidget = this;

		PinNumbersTileView->AddItem(CreatedPinEntryObject);
	}

	CurrentlyEnteredPin.Empty();
	CurrentlyEnteredPin_Text.Empty();
	PinCodeText->SetText(FText::FromString(CurrentlyEnteredPin_Text));
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

	if (!bClickedByOwner)
		return;

	ActorWithWidget->PinCorrect();

	if (IsValid(ActorWithWidget->GetOtherPanelWidget(this)))
	{
		ActorWithWidget->GetOtherPanelWidget(this)->PinIsCorrect(false);
	}

	OnClickedInteractDoorButton();
}

void UDoorPanelWidget::RestartDoorPanelWidget()
{
	InteractDoorButton->SetIsEnabled(true);
	InteractDoorButton->SetVisibility(ESlateVisibility::Visible);
	InteractDoorText->SetVisibility(ESlateVisibility::Visible);
	InteractDoorButton->SetRenderOpacity(1.f);
	InteractDoorText->SetRenderOpacity(1.f);
}