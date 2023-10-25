// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/PlayerWidgets/YouDiedWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

void UYouDiedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RestartGameButton->OnClicked.AddDynamic(this, &UYouDiedWidget::RestartGameButton_OnClicked);
	RestartGameButton->OnHovered.AddDynamic(this, &UYouDiedWidget::RestartGameButton_OnHovered);
	RestartGameButton->OnUnhovered.AddDynamic(this, &UYouDiedWidget::RestartGameButton_OnUnhovered);

	QuitGameButton->OnClicked.AddDynamic(this, &UYouDiedWidget::QuitButton_OnClicked);
	QuitGameButton->OnHovered.AddDynamic(this, &UYouDiedWidget::QuitButton_OnHovered);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UYouDiedWidget::QuitButton_OnUnhovered);
}

void UYouDiedWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ShowWidgetAnim)
		PlayAnimationForward(ShowWidgetAnim, 1.f, true);
	UMarineRunnerGameInstance* MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	MarineRunnerGameInstance->ChangeBackgroundMusic(EMT_PauseMusic);

	if (DeathSound) UGameplayStatics::SpawnSound2D(GetWorld(), DeathSound);
}

void UYouDiedWidget::RestartGameButton_OnClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
}

void UYouDiedWidget::RestartGameButton_OnHovered()
{
	PlayAnimatonForButton(RestartGameHoveredAnim);
}

void UYouDiedWidget::RestartGameButton_OnUnhovered()
{
	PlayAnimatonForButton(RestartGameHoveredAnim, false);
}

void UYouDiedWidget::QuitButton_OnClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
}

void UYouDiedWidget::QuitButton_OnHovered()
{
	PlayAnimatonForButton(QuitHoveredAnim);
}

void UYouDiedWidget::QuitButton_OnUnhovered()
{
	PlayAnimatonForButton(QuitHoveredAnim, false);
}

void UYouDiedWidget::PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim)
{
	if (AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}
