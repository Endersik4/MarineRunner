// Copyright Adam Bartela.All Rights Reserved


#include "SelectDifficultyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "MainMenuWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

void USelectDifficultyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LeftArrowChoiceButton->OnClicked.AddDynamic(this, &USelectDifficultyWidget::OnClickedLeftChoiceButton);
	LeftArrowChoiceButton->OnHovered.AddDynamic(this, &USelectDifficultyWidget::OnHoveredLeftChoiceButton);
	LeftArrowChoiceButton->OnUnhovered.AddDynamic(this, &USelectDifficultyWidget::OnUnhoveredLeftChoiceButton);

	RightArrowChoiceButton->OnClicked.AddDynamic(this, &USelectDifficultyWidget::OnClickedRightChoiceButton);
	RightArrowChoiceButton->OnHovered.AddDynamic(this, &USelectDifficultyWidget::OnHoveredRightChoiceButton);
	RightArrowChoiceButton->OnUnhovered.AddDynamic(this, &USelectDifficultyWidget::OnUnhoveredRightChoiceButton);

	StartNewGameButton->OnClicked.AddDynamic(this, &USelectDifficultyWidget::OnClickedStartNewGameButton);
	StartNewGameButton->OnHovered.AddDynamic(this, &USelectDifficultyWidget::OnHoveredStartNewGameButton);
	StartNewGameButton->OnUnhovered.AddDynamic(this, &USelectDifficultyWidget::OnUnhoveredStartNewGameButton);

	FWidgetAnimationDynamicEvent HideWidgetDynamicEvent;
	HideWidgetDynamicEvent.BindDynamic(this, &USelectDifficultyWidget::OnAnimFinished_HideWidget);
	BindToAnimationFinished(HideWidgetAnim, HideWidgetDynamicEvent);
}

void USelectDifficultyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayAnimationForward(ShowWidgetAnim);
	SetGameInstance();
}

void USelectDifficultyWidget::OnClickedLeftChoiceButton()
{
	if (CurrentDifficulty - 1 < 0)
		return;

	CurrentDifficulty--;
	UpdateDifficultyText();
}

void USelectDifficultyWidget::OnHoveredLeftChoiceButton()
{
	PlayAnimatonForButton(LeftArrowChoiceHoverAnim);
}

void USelectDifficultyWidget::OnUnhoveredLeftChoiceButton()
{
	PlayAnimatonForButton(LeftArrowChoiceHoverAnim, false);
}

void USelectDifficultyWidget::OnClickedRightChoiceButton()
{
	if (CurrentDifficulty + 1 >= GameInstance->GetAllGameDifficulties().Num())
		return;

	CurrentDifficulty++;
	UpdateDifficultyText();
}

void USelectDifficultyWidget::OnHoveredRightChoiceButton()
{
	PlayAnimatonForButton(RightArrowChoiceHoverAnim);
}

void USelectDifficultyWidget::OnUnhoveredRightChoiceButton()
{
	PlayAnimatonForButton(RightArrowChoiceHoverAnim, false);
}

void USelectDifficultyWidget::OnClickedStartNewGameButton()
{
	UpdateGameDifficultyInGameInstance();

	if (CallNewGameFunc)
		CallMainMenuMusicFadeOutFunc();
	
	PlayAnimationForward(HideWidgetAnim);
}

void USelectDifficultyWidget::OnAnimFinished_HideWidget()
{
	if (!CallNewGameFunc)
		return;

	CallNewGameFunc();
}

void USelectDifficultyWidget::OnHoveredStartNewGameButton()
{
	PlayAnimatonForButton(StartNewGameHoverAnim);
}

void USelectDifficultyWidget::OnUnhoveredStartNewGameButton()
{
	PlayAnimatonForButton(StartNewGameHoverAnim, false);
}

void USelectDifficultyWidget::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim)
{
	if (!AnimToPlay)
		return;

	if (bPlayForwardAnim)
		PlayAnimationForward(AnimToPlay);
	else
		PlayAnimationReverse(AnimToPlay);
}
#pragma region /////// GAME DIFFICULTY ////////
void USelectDifficultyWidget::UpdateDifficultyText()
{
	if (!IsValid(GameInstance))
		return;

	if (GameInstance->GetAllGameDifficulties().Num() <= CurrentDifficulty)
		return;

	CurrentDifficultyLevelText->SetText(GameInstance->GetAllGameDifficulties()[CurrentDifficulty].DifficultyName);
	CurrentDifficultyDescriptionText->SetText(GameInstance->GetAllGameDifficulties()[CurrentDifficulty].DifficultyDescription);
}

void USelectDifficultyWidget::UpdateGameDifficultyInGameInstance()
{
	GameInstance->ReplaceValueInSavedSettingByName(GameInstance->GetAllGameDifficulties()[CurrentDifficulty].DifficultyLevel, GameInstance->GetGameDifficultySavedFieldName());
	GameInstance->SetCurrentGameDifficulty(GameInstance->GetAllGameDifficulties()[CurrentDifficulty]);
	GameInstance->SaveCustomSavedSettingsToConfig();
}

void USelectDifficultyWidget::SetGameInstance()
{
	TObjectPtr<UGameInstance> TempGameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!IsValid(TempGameInstance))
		return;

	GameInstance = Cast<UMarineRunnerGameInstance>(TempGameInstance);
	CurrentDifficulty = FMath::RoundToInt32(GameInstance->GetAllGameDifficulties().Num()/2.f)-1;
}
#pragma endregion