// Copyright Adam Bartela.All Rights Reserved

#include "ConfirmLoadingGameWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/PauseMenu/PauseMenuWidget.h"
#include "MarineRunner/Player/PauseMenu/PauseMenuComponent.h"

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
	if (!IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		return;

	TObjectPtr<AMarineCharacter> Player = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!IsValid(Player))
		return;

	if (!IsValid(Player->GetPauseMenuComponent()->GetPauseMenuWidget()))
		return;

	if (bShouldDeleteExistingOne)
	{
		Player->GetPauseMenuComponent()->GetPauseMenuWidget()->CurrentSpawnedMenuWidgets.Pop();
	}
	else
	{
		Player->GetPauseMenuComponent()->GetPauseMenuWidget()->CurrentSpawnedMenuWidgets.Add(FVisiblePauseMenu(this, [this](bool b) { this->BackToLoadGame(b); }));
	}
}

void UConfirmLoadingGameWidget::BackToLoadGame(bool bShouldBack)
{
	if (bShouldBack)
	{
		AddThisWidgetToCurrentSpawnedMenuWidgets(true);
		RemoveFromParent();
	}
}

#pragma region //////////// YES BUTTON ////////////////
void UConfirmLoadingGameWidget::YesButton_OnClicked()
{
	if (ConfirmType == ECT_QuitGame)
	{
		if (!IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
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
	if (!IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(GameInstance))
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

void UConfirmLoadingGameWidget::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim)
{
	if (!AnimToPlay) 
		return;

	if (bPlayForwardAnim) 
		PlayAnimationForward(AnimToPlay);
	else 
		PlayAnimationReverse(AnimToPlay);
}

void UConfirmLoadingGameWidget::SetSlotAndLevelName(const FString& SlotName, const FString& LevelName)
{
	SlotNameToLoad = SlotName;
	LevelNameToLoad = LevelName;
}

