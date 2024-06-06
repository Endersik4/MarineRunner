// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/GameMenu/GameMenuBase.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "SettingsMenu/SettingsMenuWidget.h"
#include "LoadGameMenu/LoadGameMenuWidget.h"
#include "ConfirmOptionWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

void UGameMenuBase::NativeConstruct()
{
	Super::NativeConstruct();

	LoadGameButton->OnClicked.AddDynamic(this, &UGameMenuBase::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &UGameMenuBase::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &UGameMenuBase::OnUnhoveredLoadGameButton);

	SettingsButton->OnClicked.AddDynamic(this, &UGameMenuBase::OnClickedSettingsButton);
	SettingsButton->OnHovered.AddDynamic(this, &UGameMenuBase::OnHoveredSettingsButton);
	SettingsButton->OnUnhovered.AddDynamic(this, &UGameMenuBase::OnUnhoveredSettingsButton);

	QuitGameButton->OnClicked.AddDynamic(this, &UGameMenuBase::OnClickedQuitGameButton);
	QuitGameButton->OnHovered.AddDynamic(this, &UGameMenuBase::OnHoveredQuitGameButton);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UGameMenuBase::OnUnhoveredQuitGameButton);
}

void UGameMenuBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)), TEXT("Player Controller is nullptr in PauseMenuWidget!")))
	{
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	if (IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}


	AddAllMenuButtonsToArray();
}

void UGameMenuBase::AddAllMenuButtonsToArray()
{
	AllMenuButtons.Add(LoadGameButton);
	AllMenuButtons.Add(SettingsButton);
	AllMenuButtons.Add(QuitGameButton);
}

#pragma region //////////////// LOAD GAME /////////////////
void UGameMenuBase::OnClickedLoadGameButton()
{
	;
}

void UGameMenuBase::SpawnLoadGameMenuWidget()
{
	if (!IsValid(PlayerController))
		return;

	LoadGameMenuWidget = Cast<ULoadGameMenuWidget>(CreateWidget(PlayerController, LoadGameMenuWidgetClass));
	if (!IsValid(LoadGameMenuWidget))
		return;

	EnableAllMenuButtons(false, LoadGameButton);
	LoadGameMenuWidget->AddToViewport();
	LoadGameMenuWidget->SetCurrentSpawnedMenu(this);
	LoadGameMenuWidget->FillAllSavesToLoadGameListView();

	CurrentSpawnedMenuWidgets.Add(FVisiblePauseMenu(LoadGameMenuWidget, [this](bool b) { this->RemoveLoadGameMenuWidgetFromViewport(b); }));

	bWasLoadGameMenuWidgetSpawned = true;
}

void UGameMenuBase::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
{
	if (!IsValid(LoadGameMenuWidget))
		return;

	EnableAllMenuButtons(true, LoadGameButton);

	LoadGameMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Pop();

	LoadGameMenuWidget = nullptr;

	bWasLoadGameMenuWidgetSpawned = false;
	if (bUnhoverTextLoadGame)
		OnUnhoveredLoadGameButton();
}

void UGameMenuBase::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, true, bWasLoadGameMenuWidgetSpawned);
}

void UGameMenuBase::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false, bWasLoadGameMenuWidgetSpawned);
}
#pragma endregion 

#pragma region //////////////// SETTINGS /////////////////
void UGameMenuBase::OnClickedSettingsButton()
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

void UGameMenuBase::SpawnSettingsMenuWidget()
{
	if (!IsValid(PlayerController))
		return;

	SettingsMenuWidget = Cast<USettingsMenuWidget>(CreateWidget(PlayerController, SettingsMenuWidgetClass));
	if (!IsValid(SettingsMenuWidget))
		return;

	EnableAllMenuButtons(false, SettingsButton);
	SettingsMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(FVisiblePauseMenu(SettingsMenuWidget, [this](bool b) { this->RemoveSettingsMenuWidgetFromViewport(b); }));

	bWasSettingsMenuWidgetSpawned = true;
}

void UGameMenuBase::RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings)
{
	if (!IsValid(SettingsMenuWidget))
		return;

	EnableAllMenuButtons(true, SettingsButton);

	SettingsMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Pop();

	SettingsMenuWidget = nullptr;

	bWasSettingsMenuWidgetSpawned = false;
	if (bUnhoverTextSettings && !SettingsButton->IsHovered())
		OnUnhoveredSettingsButton();
}

void UGameMenuBase::OnHoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, true, bWasSettingsMenuWidgetSpawned);
}

void UGameMenuBase::OnUnhoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, false, bWasSettingsMenuWidgetSpawned);
}

#pragma endregion 

#pragma region //////////////// QUIT GAME /////////////////
void UGameMenuBase::OnClickedQuitGameButton()
{
	;
}

void UGameMenuBase::OnHoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim);
}

void UGameMenuBase::OnUnhoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim, false);
}
#pragma endregion 

bool UGameMenuBase::BackToPreviousMenu()
{
	if (CurrentSpawnedMenuWidgets.Num() == 0)
		return true;

	(CurrentSpawnedMenuWidgets.Last().FunctionToHideWidget)(true);

	return false;
}

void UGameMenuBase::EnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore)
{
	for (TObjectPtr<UButton> CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton)
			continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
	}
}

void UGameMenuBase::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || !AnimToPlay)
		return;

	if (bPlayForwardAnim)
		PlayAnimationForward(AnimToPlay);
	else
		PlayAnimationReverse(AnimToPlay);
}