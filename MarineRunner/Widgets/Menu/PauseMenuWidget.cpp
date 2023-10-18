// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Widgets/Menu/SettingsMenuWidget.h"
#include "LoadGameMenu/LoadGameMenuWidget.h"
#include "LoadGameMenu/ConfirmLoadingGameWidget.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

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

	MarinePlayer = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	MarineRunnerGameInstance->ChangeBackgroundMusic(EMT_MainMenu);

	FillMenuButtonsAndTextMap();
}

void UPauseMenuWidget::NativeDestruct()
{
	MarineRunnerGameInstance->ChangeBackgroundMusic(EMT_Exploration);
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
	if (IsValid(MarinePlayer) == false)
		return;
	if (IsValid(MarinePlayer->GetPauseMenuComponent()) == false)
		return;

	MarinePlayer->GetPauseMenuComponent()->CallUnPauseGame();
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
	if (LoadGameMenuWidget == nullptr) 
		return;

	SetEnableAllMenuButtons(false, LoadGameButton);
	LoadGameMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(LoadGameMenuWidget, [this](bool b) { this->RemoveLoadGameMenuWidgetFromViewport(b); });

	bWasLoadGameMenuWidgetSpawned = true;
}

void UPauseMenuWidget::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
{
	if (LoadGameMenuWidget == nullptr) return;

	SetEnableAllMenuButtons(true, LoadGameButton);

	LoadGameMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Remove(LoadGameMenuWidget);
	LoadGameMenuWidget = nullptr;

	bWasLoadGameMenuWidgetSpawned = false;
	if (bUnhoverTextLoadGame == true) OnUnhoveredLoadGameButton();
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
	if (IsValid(MarinePlayer) == false)
		return;

	if (MarinePlayer->CanPlayerSaveGame() == false)
	{
		MarinePlayer->SpawnCannotSavedWidget();
		return;
	}

	MarinePlayer->CallSaveGame();

	if (IsValid(MarinePlayer->GetPauseMenuComponent()) == false)
		return;

	MarinePlayer->GetPauseMenuComponent()->CallUnPauseGame();
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
	if (SettingsMenuWidget == nullptr) return;

	SetEnableAllMenuButtons(false, SettingsButton);
	SettingsMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(SettingsMenuWidget, [this](bool b) { this->RemoveSettingsMenuWidgetFromViewport(b); });

	bWasSettingsMenuWidgetSpawned = true;
}

void UPauseMenuWidget::RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings)
{
	if (SettingsMenuWidget == nullptr) return;

	SetEnableAllMenuButtons(true, SettingsButton);

	SettingsMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Remove(SettingsMenuWidget);
	SettingsMenuWidget = nullptr;

	bWasSettingsMenuWidgetSpawned = false;
	if (bUnhoverTextSettings == true && SettingsButton->IsHovered() == false) 
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
	if (IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)) == false)
		return;

	UConfirmLoadingGameWidget* ConfirmQuitingGameWidget = Cast<UConfirmLoadingGameWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), ConfirmLoadingSaveWidgetClass));
	if (IsValid(ConfirmQuitingGameWidget) == false)
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
	if (CurrentSpawnedMenuWidgets.Num() == 0) return true;

	TArray<UUserWidget*> SpawnedMenuWidgets;
	CurrentSpawnedMenuWidgets.GenerateKeyArray(SpawnedMenuWidgets);

	if (CurrentSpawnedMenuWidgets.Contains(SpawnedMenuWidgets.Last()))
	{
		TFunction<void(bool)>* DeleteWidgetFunction = CurrentSpawnedMenuWidgets.Find(SpawnedMenuWidgets.Last());
		(*DeleteWidgetFunction)(true);
		CurrentSpawnedMenuWidgets.Remove(SpawnedMenuWidgets.Last());
	}
	return false;
}

void UPauseMenuWidget::SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore)
{
	for (UButton* CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton) continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
	}
}

void UPauseMenuWidget::PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}