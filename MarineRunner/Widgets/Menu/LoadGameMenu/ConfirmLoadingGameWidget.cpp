// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Widgets/Menu/LoadGameMenu/ConfirmLoadingGameWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"

void UConfirmLoadingGameWidget::NativeConstruct()
{
	YesButton->OnClicked.AddDynamic(this, &UConfirmLoadingGameWidget::YesButton_OnClicked);
	YesButton->OnHovered.AddDynamic(this, &UConfirmLoadingGameWidget::YesButton_OnHovered);
	YesButton->OnUnhovered.AddDynamic(this, &UConfirmLoadingGameWidget::YesButton_OnUnhovered);

	NoButton->OnClicked.AddDynamic(this, &UConfirmLoadingGameWidget::NoButton_OnClicked);
	NoButton->OnHovered.AddDynamic(this, &UConfirmLoadingGameWidget::NoButton_OnHovered);
	NoButton->OnUnhovered.AddDynamic(this, &UConfirmLoadingGameWidget::NoButton_OnUnhovered);
}

void UConfirmLoadingGameWidget::NativeOnInitialized()
{
	AddThisWidgetToCurrentSpawnedMenuWidgets(false);
}

void UConfirmLoadingGameWidget::AddThisWidgetToCurrentSpawnedMenuWidgets(bool bShouldDeleteExistingOne)
{
	AMarineCharacter* MarinePlayer = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (IsValid(MarinePlayer) == false)
		return;

	if (IsValid(MarinePlayer->GetPauseMenuComponent()->GetPauseMenuWidget()) == false)
		return;

	if (bShouldDeleteExistingOne == true)
	{
		MarinePlayer->GetPauseMenuComponent()->GetPauseMenuWidget()->CurrentSpawnedMenuWidgets.Remove(this);
	}
	else
	{
		MarinePlayer->GetPauseMenuComponent()->GetPauseMenuWidget()->CurrentSpawnedMenuWidgets.Add(this, [this](bool b) { this->BackToLoadGame(b); });
	}
}

void UConfirmLoadingGameWidget::BackToLoadGame(bool bShouldBack)
{
	if (bShouldBack) 
		RemoveFromParent();
}

#pragma region //////////// YES BUTTON ////////////////
void UConfirmLoadingGameWidget::YesButton_OnClicked()
{
	if (ConfirmType == ECT_QuitGame)
	{
		if (IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)) == false)
			return;

		UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
	}
	else
	{
		LoadLastSave();
	}
}

void UConfirmLoadingGameWidget::LoadLastSave()
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance) == false)
		return;

	GameInstance->SlotSaveGameNameToLoad = SlotNameToLoad;

	UGameplayStatics::OpenLevel(GetWorld(), FName(LevelNameToLoad));
}

void UConfirmLoadingGameWidget::YesButton_OnHovered()
{
	PlayAnimatonForButton(YesHoveredAnim);
}

void UConfirmLoadingGameWidget::YesButton_OnUnhovered()
{
	PlayAnimatonForButton(YesHoveredAnim, false);
}
#pragma endregion

#pragma region ////////////// NO BUTTON /////////////
void UConfirmLoadingGameWidget::NoButton_OnClicked()
{
	AddThisWidgetToCurrentSpawnedMenuWidgets(true);
	RemoveFromParent();
}

void UConfirmLoadingGameWidget::NoButton_OnHovered()
{
	PlayAnimatonForButton(NoHoveredAnim);
}

void UConfirmLoadingGameWidget::NoButton_OnUnhovered()
{
	PlayAnimatonForButton(NoHoveredAnim, false);
}

#pragma endregion

void UConfirmLoadingGameWidget::PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim)
{
	if (AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}

void UConfirmLoadingGameWidget::SetSlotAndLevelName(const FString& SlotName, const FString& LevelName)
{
	SlotNameToLoad = SlotName;
	LevelNameToLoad = LevelName;
}

