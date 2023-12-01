// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/MainMenu/MainMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Widgets/Menu/SettingsMenuWidget.h"
#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "MarineRunner/Widgets/Menu/LoadGameMenu/ConfirmLoadingGameWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"


void UMainMenuWidget::NativeConstruct()
{
	ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedContinueButton);
	ContinueButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredContinueButton);
	ContinueButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredContinueButton);

	NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedNewGameButton);
	NewGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredNewGameButton);
	NewGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredNewGameButton);

	LoadGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredLoadGameButton);

	SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedSettingsButton);
	SettingsButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredSettingsButton);
	SettingsButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredSettingsButton);

	QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedQuitGameButton);
	QuitGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredQuitGameButton);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredQuitGameButton);
}

void UMainMenuWidget::NativeOnInitialized()
{
	FillMenuButtonsAndTextMap();
}

void UMainMenuWidget::OnClickedContinueButton()
{
}

void UMainMenuWidget::OnHoveredContinueButton()
{
	PlayAnimatonForButton(ContinueHoverAnim);

}

void UMainMenuWidget::OnUnhoveredContinueButton()
{
	PlayAnimatonForButton(ContinueHoverAnim, false);
}

void UMainMenuWidget::OnClickedNewGameButton()
{
}

void UMainMenuWidget::OnHoveredNewGameButton()
{
	PlayAnimatonForButton(NewGameHoverAnim);

}

void UMainMenuWidget::OnUnhoveredNewGameButton()
{
	PlayAnimatonForButton(NewGameHoverAnim, false);
}

void UMainMenuWidget::OnClickedLoadGameButton()
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

void UMainMenuWidget::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, true, bWasLoadGameMenuWidgetSpawned);

}

void UMainMenuWidget::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false, bWasLoadGameMenuWidgetSpawned);

}

void UMainMenuWidget::OnClickedSettingsButton()
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

void UMainMenuWidget::SpawnLoadGameMenuWidget()
{
	LoadGameMenuWidget = Cast<ULoadGameMenuWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), LoadGameMenuWidgetClass));
	if (LoadGameMenuWidget == nullptr)
		return;

	SetEnableAllMenuButtons(false, LoadGameButton);
	LoadGameMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(LoadGameMenuWidget, [this](bool b) { this->RemoveLoadGameMenuWidgetFromViewport(b); });

	bWasLoadGameMenuWidgetSpawned = true;
}

void UMainMenuWidget::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
{
	if (LoadGameMenuWidget == nullptr) return;

	SetEnableAllMenuButtons(true, LoadGameButton);

	LoadGameMenuWidget->RemoveFromParent();
	CurrentSpawnedMenuWidgets.Remove(LoadGameMenuWidget);
	LoadGameMenuWidget = nullptr;

	bWasLoadGameMenuWidgetSpawned = false;
	if (bUnhoverTextLoadGame == true) OnUnhoveredLoadGameButton();
}

void UMainMenuWidget::SpawnSettingsMenuWidget()
{
	SettingsMenuWidget = Cast<USettingsMenuWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), SettingsMenuWidgetClass));
	if (SettingsMenuWidget == nullptr) return;

	SetEnableAllMenuButtons(false, SettingsButton);
	SettingsMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(SettingsMenuWidget, [this](bool b) { this->RemoveSettingsMenuWidgetFromViewport(b); });

	bWasSettingsMenuWidgetSpawned = true;
}

void UMainMenuWidget::RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings)
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

void UMainMenuWidget::FillMenuButtonsAndTextMap()
{
	AllMenuButtons.Add(ContinueButton);
	AllMenuButtons.Add(NewGameButton);
	AllMenuButtons.Add(LoadGameButton);
	AllMenuButtons.Add(SettingsButton);
	AllMenuButtons.Add(QuitGameButton);
}

void UMainMenuWidget::SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore)
{
	for (UButton* CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton) continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
	}
}

void UMainMenuWidget::OnHoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, true, bWasSettingsMenuWidgetSpawned);
}

void UMainMenuWidget::OnUnhoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, false, bWasSettingsMenuWidgetSpawned);
}

void UMainMenuWidget::OnClickedQuitGameButton()
{
	if (IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)) == false)
		return;

	UConfirmLoadingGameWidget* ConfirmQuitingGameWidget = Cast<UConfirmLoadingGameWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), ConfirmLoadingSaveWidgetClass));
	if (IsValid(ConfirmQuitingGameWidget) == false)
		return;

	ConfirmQuitingGameWidget->AddToViewport();
	ConfirmQuitingGameWidget->SetConfirmType(ECT_QuitGame);
}

void UMainMenuWidget::OnHoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim);
}

void UMainMenuWidget::OnUnhoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim, false);
}

bool UMainMenuWidget::RemoveCurrentMenuWidgetsFromViewport()
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

void UMainMenuWidget::PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}
