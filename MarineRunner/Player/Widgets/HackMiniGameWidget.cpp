// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/Widgets/HackMiniGameWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void UHackMiniGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ChooseNumberButton->OnClicked.AddDynamic(this, &UHackMiniGameWidget::OnClicked_ChooseNumberButton);

	AllPinTextBlocks.Add(FirstPinNumber);
	AllPinTextBlocks.Add(SecondPinNumber);
	AllPinTextBlocks.Add(ThirdPinNumber);
	AllPinTextBlocks.Add(FourPinNumber);
}

void UHackMiniGameWidget::OnClicked_ChooseNumberButton()
{
	if (bGameStarted)
	{
		if (bCorrectNumberVisible)
		{
			CurrentIndexOfPin++;
			bCorrectNumberVisible = false;
			if (CurrentIndexOfPin > 3)
			{
				AllPinTextBlocks[0]->SetText(FText::FromString("*"));
				AllPinTextBlocks[1]->SetText(FText::FromString("*"));
				AllPinTextBlocks[2]->SetText(FText::FromString("*"));
				AllPinTextBlocks[3]->SetText(FText::FromString("*"));
				CurrentIndexOfPin = 0;
				bGameStarted = false;
			}
		}
		else
		{
			if (CurrentIndexOfPin != 0)
			{
				AllPinTextBlocks[CurrentIndexOfPin]->SetText(FText::FromString("*"));
				CurrentIndexOfPin--;
			}
			else
				return;
		}
	}
	else
		bGameStarted = true;
	
	CurrentCorrectNumber = FCString::Atoi(*FString::Chr(CorrectPin[CurrentIndexOfPin]));

	GetWorld()->GetTimerManager().SetTimer(UpdateCurrentNumberHandle, this, &UHackMiniGameWidget::UpdateCurrentNumber, NormalNumberVisibleTime, true);

	GetWorld()->GetTimerManager().SetTimer(ShowCorrectNumberHandle, this, &UHackMiniGameWidget::ShowCorrectNumber, 
		UKismetMathLibrary::RandomFloatInRange(RandomTimeToShowCorrectNumber.GetLowerBoundValue(), RandomTimeToShowCorrectNumber.GetUpperBoundValue()), false);
}

void UHackMiniGameWidget::ShowCorrectNumber()
{
	bCorrectNumberVisible = true;

	GetWorld()->GetTimerManager().ClearTimer(UpdateCurrentNumberHandle);

	AllPinTextBlocks[CurrentIndexOfPin]->SetColorAndOpacity(CorrectNumberTextColor);
	AllPinTextBlocks[CurrentIndexOfPin]->SetText(FText::AsNumber(CurrentCorrectNumber));

	GetWorld()->GetTimerManager().SetTimer(UpdateCurrentNumberHandle, this, &UHackMiniGameWidget::UpdateCurrentNumber, NormalNumberVisibleTime, true, VisibleCorrectNumberTime);

	GetWorld()->GetTimerManager().SetTimer(ShowCorrectNumberHandle, this, &UHackMiniGameWidget::ShowCorrectNumber,
		UKismetMathLibrary::RandomFloatInRange(RandomTimeToShowCorrectNumber.GetLowerBoundValue(), RandomTimeToShowCorrectNumber.GetUpperBoundValue()), false);
}

void UHackMiniGameWidget::UpdateCurrentNumber()
{
	bCorrectNumberVisible = false;
	AllPinTextBlocks[CurrentIndexOfPin]->SetColorAndOpacity(OriginalTextColor);

	int32 RandomNumber = UKismetMathLibrary::RandomIntegerInRange(1, 9);

	if (RandomNumber == LastRandomNumber)
	{
		if (RandomNumber == 9)
			RandomNumber--;
		else
			RandomNumber++;
	}

	if (RandomNumber == CurrentCorrectNumber)
	{
		if (RandomNumber == 9)
			RandomNumber--;
		else
			RandomNumber++;
	}

	LastRandomNumber = RandomNumber;

	AllPinTextBlocks[CurrentIndexOfPin]->SetText(FText::AsNumber(RandomNumber));
}
