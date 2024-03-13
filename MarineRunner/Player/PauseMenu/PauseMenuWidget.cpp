// Copyright Adam Bartela.All Rights Reserved


#include "PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "SettingsMenu/SettingsMenuWidget.h"
#include "LoadGameMenu/LoadGameMenuWidget.h"
#include "LoadGameMenu/ConfirmLoadingGameWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/PauseMenu/PauseMenuComponent.h"
#include "MarineRunner/Player/SaveLoadGame/SaveLoadPlayerComponent.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedResumeButton);
	ResumeButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredResumeButton);
	ResumeButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredResumeButton);

	LoadGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredLoadGameButton);

	SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedSettingsButton);
	SettingsButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredSettingsButton);
	SettingsButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredSettingsButton);

	SaveGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedSaveGameButton);
	SaveGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredSaveGameButton);
	SaveGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredSaveGameButton);

	QuitGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedQuitGameButton);
	QuitGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredQuitGameButton);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredQuitGameButton);
}

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Player = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	PlayPauseMenuMusic();

	FillMenuButtonsAndTextMap();

	if (ShowPauseWidgetAnim)
		PlayAnimationForward(ShowPauseWidgetAnim, 1.f, true);
}

void UPauseMenuWidget::NativeDestruct()
{
	StopPauseMenuMusic();
}

void UPauseMenuWidget::PlayPauseMenuMusic()
{
	GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(GameInstance))
		return;

	if (GameInstance->GetCurrentMusicType() == EMT_Combat)
		return;

	MusicTypeBeforePause = GameInstance->GetCurrentMusicType();
	GameInstance->ChangeBackgroundMusic(EMT_PauseMusic, true);
	if (!IsValid(PauseMenuMusic))
		return;

	CurrentPauseMenuMusic = UGameplayStatics::SpawnSound2D(GetWorld(), PauseMenuMusic);
	CurrentPauseMenuMusic->FadeIn(1.f);
}

void UPauseMenuWidget::StopPauseMenuMusic()
{
	if (IsValid(GameInstance))
	{
		if (GameInstance->GetCurrentMusicType() == EMT_Combat)
			return;

		GameInstance->ChangeBackgroundMusic(MusicTypeBeforePause, true);
	}

	if (IsValid(CurrentPauseMenuMusic) == false) 
		return;

	CurrentPauseMenuMusic->Stop();
	CurrentPauseMenuMusic->FadeOut(0.f, 0.f);
}

void UPauseMenuWidget::FillMenuButtonsAndTextMap()
{
	AllMenuButtons.Add(ResumeButton);
	AllMenuButtons.Add(LoadGameButton);
	AllMenuButtons.Add(SettingsButton);
	AllMenuButtons.Add(SaveGameButton);
	AllMenuButtons.Add(QuitGameButton);
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

#pragma region //////////////// LOAD GAME /////////////////
void UPauseMenuWidget::OnClickedLoadGameButton()
{
	if (bWasLoadGameMenuWidgetSpawned)
	{
		RemoveLoadGameMenuWidgetFromViewport();
	}
	else
	{
		SpawnLoadGameMenuWidget();
	}
}

void UPauseMenuWidget::SpawnLoadGameMenuWidget()
{
	LoadGameMenuWidget = Cast<ULoadGameMenuWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), LoadGameMenuWidgetClass));
	if (!IsValid(LoadGameMenuWidget)) 
		return;

	SetEnableAllMenuButtons(false, LoadGameButton);
	LoadGameMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(LoadGameMenuWidget, [this](bool b) { this->RemoveLoadGameMenuWidgetFromViewport(b); });

	bWasLoadGameMenuWidgetSpawned = true;
}

void UPauseMenuWidget::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
{
	if (!IsValid(LoadGameMenuWidget)) 
		return;

	SetEnableAllMenuButtons(true, LoadGameButton);

	LoadGameMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Remove(LoadGameMenuWidget);
	LoadGameMenuWidget = nullptr;

	bWasLoadGameMenuWidgetSpawned = false;
	if (bUnhoverTextLoadGame) 
		OnUnhoveredLoadGameButton();
}

void UPauseMenuWidget::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, true, bWasLoadGameMenuWidgetSpawned);
}

void UPauseMenuWidget::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false, bWasLoadGameMenuWidgetSpawned);
}
#pragma endregion 

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


#pragma region //////////////// SETTINGS /////////////////
void UPauseMenuWidget::OnClickedSettingsButton()
{
	if (bWasSettingsMenuWidgetSpawned)
	{
		RemoveSettingsMenuWidgetFromViewport();
	}
	else
	{
		SpawnSettingsMenuWidget();
	}
}

void UPauseMenuWidget::SpawnSettingsMenuWidget()
{
	SettingsMenuWidget = Cast<USettingsMenuWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), SettingsMenuWidgetClass));
	if (!IsValid(SettingsMenuWidget)) 
		return;

	SetEnableAllMenuButtons(false, SettingsButton);
	SettingsMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(SettingsMenuWidget, [this](bool b) { this->RemoveSettingsMenuWidgetFromViewport(b); });

	bWasSettingsMenuWidgetSpawned = true;
}

void UPauseMenuWidget::RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings)
{
	if (!IsValid(SettingsMenuWidget)) 
		return;

	SetEnableAllMenuButtons(true, SettingsButton);

	SettingsMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Remove(SettingsMenuWidget);
	SettingsMenuWidget = nullptr;

	bWasSettingsMenuWidgetSpawned = false;
	if (bUnhoverTextSettings && !SettingsButton->IsHovered()) 
		OnUnhoveredSettingsButton();
}

void UPauseMenuWidget::OnHoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, true, bWasSettingsMenuWidgetSpawned);
}

void UPauseMenuWidget::OnUnhoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, false, bWasSettingsMenuWidgetSpawned);
}

#pragma endregion 

#pragma region //////////////// QUIT GAME /////////////////
void UPauseMenuWidget::OnClickedQuitGameButton()
{
	if (!IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		return;

	TObjectPtr<UConfirmLoadingGameWidget> ConfirmQuitingGameWidget = Cast<UConfirmLoadingGameWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), ConfirmLoadingSaveWidgetClass));
	if (!IsValid(ConfirmQuitingGameWidget))
		return;

	ConfirmQuitingGameWidget->AddToViewport();
	ConfirmQuitingGameWidget->SetConfirmType(ECT_QuitGame);
}

void UPauseMenuWidget::OnHoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim, false);
}
#pragma endregion 

bool UPauseMenuWidget::RemoveCurrentMenuWidgetsFromViewport()
{
	if (CurrentSpawnedMenuWidgets.Num() == 0) 
		return true;

	TArray<TObjectPtr<UUserWidget>> SpawnedMenuWidgets;
	CurrentSpawnedMenuWidgets.GenerateKeyArray(SpawnedMenuWidgets);

	if (CurrentSpawnedMenuWidgets.Contains(SpawnedMenuWidgets.Last()))
	{
		TFunction<void(bool)>* DeleteWidgetFunction = CurrentSpawnedMenuWidgets.Find(SpawnedMenuWidgets.Last());
		(*DeleteWidgetFunction)(true);
		CurrentSpawnedMenuWidgets.Remove(SpawnedMenuWidgets.Last());
	}
	return false;
}

void UPauseMenuWidget::SetEnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore)
{
	for (TObjectPtr<UButton> CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton) 
			continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
	}
}

void UPauseMenuWidget::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || !AnimToPlay)
		return;

	if (bPlayForwardAnim)
		PlayAnimationForward(AnimToPlay);
	else
		PlayAnimationReverse(AnimToPlay);
}