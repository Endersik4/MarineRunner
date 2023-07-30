// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/FrontChestPanelWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MarineRunner/Objects/ChestWithItems.h"

void UFrontChestPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OpenButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::OpenButtonPressed);
	DeleteButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::DeleteButtonPressed);

	NumberOneButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberOnePressed);
	NumberTwoButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberTwoPressed);
	NumberThreeButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberThreePressed);
	NumberFourButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberFourPressed);
	NumberFiveButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberFivePressed);
	NumberSixButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberSixPressed);
	NumberSevenButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberSevenPressed);
	NumberEightButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberEightPressed);
	NumberNineButton->OnClicked.AddDynamic(this, &UFrontChestPanelWidget::NumberNinePressed);
}

void UFrontChestPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UFrontChestPanelWidget::OpenButtonPressed()
{
	if (ChestActor == nullptr) return;

	ChestActor->OpenChest(PinCodeString);
}

void UFrontChestPanelWidget::DeleteButtonPressed()
{
	if (PinCodeString.Len() == 0) return;

	PinCodeString.RemoveAt(PinCodeString.Len() - 1);
	PinCodeNotVisibleString.RemoveAt(PinCodeNotVisibleString.Len() - 1);
	PINText->SetText(FText::FromString(PinCodeNotVisibleString));

	OpenButton->SetIsEnabled(false);
}

void UFrontChestPanelWidget::DisableOpenButton(bool bDisable)
{
	OpenButton->SetIsEnabled(bDisable);

	if (bDisable == true)
	{
		DeleteButton->SetIsEnabled(false);
		PINText->SetText(FText::FromString("****"));
	}
}

void UFrontChestPanelWidget::ResetPinCodeStrings()
{
	PINText->SetText(FText::FromString(""));
	OpenButton->SetIsEnabled(false);
	PinCodeString = "";
	PinCodeNotVisibleString = "";
}

#pragma region //////////// PIN CODE CLICKED ////////////////////
void UFrontChestPanelWidget::PinNumbersClicked(FString Number)
{
	if (PinCodeString.Len() < 4 && OpenButton->GetIsEnabled() == false)
	{
		PinCodeString += Number;
		PinCodeNotVisibleString += "*";
		PINText->SetText(FText::FromString(PinCodeNotVisibleString));

		if (PinCodeString.Len() == 4) OpenButton->SetIsEnabled(true);
	}
}

void UFrontChestPanelWidget::NumberOnePressed()
{
	PinNumbersClicked("1");
}

void UFrontChestPanelWidget::NumberTwoPressed()
{
	PinNumbersClicked("2");
}

void UFrontChestPanelWidget::NumberThreePressed()
{
	PinNumbersClicked("3");
}

void UFrontChestPanelWidget::NumberFourPressed()
{
	PinNumbersClicked("4");
}

void UFrontChestPanelWidget::NumberFivePressed()
{
	PinNumbersClicked("5");
}

void UFrontChestPanelWidget::NumberSixPressed()
{
	PinNumbersClicked("6");
}

void UFrontChestPanelWidget::NumberSevenPressed()
{
	PinNumbersClicked("7");
}

void UFrontChestPanelWidget::NumberEightPressed()
{
	PinNumbersClicked("8");
}

void UFrontChestPanelWidget::NumberNinePressed()
{
	PinNumbersClicked("9");
}

#pragma endregion