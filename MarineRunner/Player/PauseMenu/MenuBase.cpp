// Copyright Adam Bartela.All Rights Reserved

#include "MenuBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "SettingsMenu/SettingsMenuWidget.h"
#include "LoadGameMenu/LoadGameMenuWidget.h"
#include "MarineRunner/Player/PauseMenu/ConfirmOptionWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

void UMenuBase::NativeConstruct()
{
	Super::NativeConstruct();

	LoadGameButton->OnClicked.AddDynamic(this, &UMenuBase::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &UMenuBase::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &UMenuBase::OnUnhoveredLoadGameButton);

	SettingsButton->OnClicked.AddDynamic(this, &UMenuBase::OnClickedSettingsButton);
	SettingsButton->OnHovered.AddDynamic(this, &UMenuBase::OnHoveredSettingsButton);
	SettingsButton->OnUnhovered.AddDynamic(this, &UMenuBase::OnUnhoveredSettingsButton);

	QuitGameButton->OnClicked.AddDynamic(this, &UMenuBase::OnClickedQuitGameButton);
	QuitGameButton->OnHovered.AddDynamic(this, &UMenuBase::OnHoveredQuitGameButton);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UMenuBase::OnUnhoveredQuitGameButton);
}

void UMenuBase::NativeOnInitialized()
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

void UMenuBase::AddAllMenuButtonsToArray()
{
	AllMenuButtons.Add(LoadGameButton);
	AllMenuButtons.Add(SettingsButton);
	AllMenuButtons.Add(QuitGameButton);
}

#pragma region //////////////// LOAD GAME /////////////////
void UMenuBase::OnClickedLoadGameButton()
{
	;
}

void UMenuBase::SpawnLoadGameMenuWidget()
{
	if (!IsValid(PlayerController))
		return;

	LoadGameMenuWidget = Cast<ULoadGameMenuWidget>(CreateWidget(PlayerController, LoadGameMenuWidgetClass));
	if (!IsValid(LoadGameMenuWidget))
		return;

	EnableAllMenuButtons(false, LoadGameButton);
	LoadGameMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(FVisiblePauseMenu(LoadGameMenuWidget, [this](bool b) { this->RemoveLoadGameMenuWidgetFromViewport(b); }));

	bWasLoadGameMenuWidgetSpawned = true;
}

void UMenuBase::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
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

void UMenuBase::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, true, bWasLoadGameMenuWidgetSpawned);
}

void UMenuBase::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false, bWasLoadGameMenuWidgetSpawned);
}
#pragma endregion 

#pragma region //////////////// SETTINGS /////////////////
void UMenuBase::OnClickedSettingsButton()
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

void UMenuBase::SpawnSettingsMenuWidget()
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

void UMenuBase::RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings)
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

void UMenuBase::OnHoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, true, bWasSettingsMenuWidgetSpawned);
}

void UMenuBase::OnUnhoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, false, bWasSettingsMenuWidgetSpawned);
}

#pragma endregion 

#pragma region //////////////// QUIT GAME /////////////////
void UMenuBase::OnClickedQuitGameButton()
{
	;
}

void UMenuBase::OnHoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim);
}

void UMenuBase::OnUnhoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim, false);
}
#pragma endregion 

bool UMenuBase::BackToPreviousMenu()
{
	if (CurrentSpawnedMenuWidgets.Num() == 0)
		return true;

	(CurrentSpawnedMenuWidgets.Last().FunctionToHideWidget)(true);

	return false;
}

void UMenuBase::EnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore)
{
	for (TObjectPtr<UButton> CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton)
			continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
	}
}

void UMenuBase::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || !AnimToPlay)
		return;

	if (bPlayForwardAnim)
		PlayAnimationForward(AnimToPlay);
	else
		PlayAnimationReverse(AnimToPlay);
}