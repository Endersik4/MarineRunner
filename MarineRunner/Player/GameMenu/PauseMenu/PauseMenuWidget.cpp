// Copyright Adam Bartela.All Rights Reserved


#include "PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Player/GameMenu/SettingsMenu/SettingsMenuWidget.h"
#include "MarineRunner/Player/GameMenu/LoadGameMenu/LoadGameMenuWidget.h"
#include "MarineRunner/Player/GameMenu/ConfirmOptionWidget.h"
#include "MarineRunner/Player/GameMenu/PauseMenu/PauseMenuComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/SaveLoadGame/SaveLoadPlayerComponent.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedResumeButton);
	ResumeButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredResumeButton);
	ResumeButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredResumeButton);

	SaveGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedSaveGameButton);
	SaveGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredSaveGameButton);
	SaveGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredSaveGameButton);
}

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)), TEXT("Player is nullptr in PauseMenuWidget!")))
	{
		Player = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}

	PlayPauseMenuMusic();

	AllMenuButtons.Add(ResumeButton);
	AllMenuButtons.Add(SaveGameButton);

	if (ShowPauseWidgetAnim)
		PlayAnimationForward(ShowPauseWidgetAnim, 1.f, true);
}

void UPauseMenuWidget::NativeDestruct()
{
	StopPauseMenuMusic();
}

void UPauseMenuWidget::PlayPauseMenuMusic()
{
	if (!IsValid(GameInstance))
		return;

	if (GameInstance->GetCurrentMusicType() == EMT_Combat)
		return;

	MusicTypeBeforePause = GameInstance->GetCurrentMusicType();
	GameInstance->ChangeBackgroundMusic(EMT_PauseMusic, true);
	if (!IsValid(PauseMenuMusic))
		return;

	CurrentMenuMusic = UGameplayStatics::SpawnSound2D(GetWorld(), PauseMenuMusic);
	CurrentMenuMusic->FadeIn(1.f);
}

void UPauseMenuWidget::StopPauseMenuMusic()
{
	if (IsValid(GameInstance))
	{
		if (GameInstance->GetCurrentMusicType() == EMT_Combat)
			return;

		GameInstance->ChangeBackgroundMusic(MusicTypeBeforePause, true);
	}

	if (!IsValid(CurrentMenuMusic)) 
		return;

	CurrentMenuMusic->Stop();
	CurrentMenuMusic->FadeOut(0.f, 0.f);
}

#pragma region //////////////// RESUME /////////////////
void UPauseMenuWidget::OnClickedResumeButton()
{
	if (!IsValid(Player))
		return;
	if (!IsValid(Player->GetPauseMenuComponent()))
		return;

	Player->GetPauseMenuComponent()->UnPauseGame();
}

void UPauseMenuWidget::OnHoveredResumeButton()
{
	PlayAnimatonForButton(ResumeHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredResumeButton()
{
	PlayAnimatonForButton(ResumeHoverAnim, false);
}
#pragma endregion 

void UPauseMenuWidget::OnClickedLoadGameButton()
{
	Super::OnClickedLoadGameButton();

	if (bWasLoadGameMenuWidgetSpawned)
	{
		RemoveLoadGameMenuWidgetFromViewport();
	}
	else
	{
		SpawnLoadGameMenuWidget();
	}
}

#pragma region //////////////// SAVE GAME /////////////////
void UPauseMenuWidget::OnClickedSaveGameButton()
{
	if (!IsValid(Player))
		return;

	if (!Player->GetSaveLoadPlayerComponent()->CanPlayerSaveGame())
	{
		Player->GetSaveLoadPlayerComponent()->SpawnCannotSavedWidget();
		return;
	}

	Player->GetSaveLoadPlayerComponent()->SaveGame();

	if (!IsValid(Player->GetPauseMenuComponent()))
		return;

	Player->GetPauseMenuComponent()->UnPauseGame();
}

void UPauseMenuWidget::OnHoveredSaveGameButton()
{
	PlayAnimatonForButton(SaveGameHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredSaveGameButton()
{
	PlayAnimatonForButton(SaveGameHoverAnim, false);
}
#pragma endregion 

#pragma region //////////////// QUIT GAME /////////////////
void UPauseMenuWidget::OnClickedQuitGameButton()
{
	Super::OnClickedQuitGameButton();

	if (!IsValid(PlayerController))
		return;

	TObjectPtr<UConfirmOptionWidget> ConfirmQuitingGameWidget = Cast<UConfirmOptionWidget>(CreateWidget(PlayerController, ConfirmOptionWidgetClass));
	if (!IsValid(ConfirmQuitingGameWidget))
		return;

	ConfirmQuitingGameWidget->AddToViewport();
	ConfirmQuitingGameWidget->ConfirmFunction = [this]() {this->QuitGame(); };
}

void UPauseMenuWidget::QuitGame()
{
	if (!PlayerController)
		return;

	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}
#pragma endregion 