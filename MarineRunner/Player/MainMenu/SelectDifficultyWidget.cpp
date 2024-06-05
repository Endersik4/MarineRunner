// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/MainMenu/SelectDifficultyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MainMenuWidget.h"

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
}

void USelectDifficultyWidget::OnClickedLeftChoiceButton()
{
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
