// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Widgets/Menu/SettingsMenuWidget.h"

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

	RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedRestartButton);
	RestartButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredRestartButton);
	RestartButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredRestartButton);

	QuitGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedQuitGameButton);
	QuitGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredQuitGameButton);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredQuitGameButton);
}

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FillMenuButtonsAndTextMap();
}

void UPauseMenuWidget::FillMenuButtonsAndTextMap()
{
	MenuButtonsAndText.Add(ResumeButton, ResumeText);
	MenuButtonsAndText.Add(LoadGameButton, LoadGameText);
	MenuButtonsAndText.Add(SettingsButton, SettingsText);
	MenuButtonsAndText.Add(RestartButton, RestartText);
	MenuButtonsAndText.Add(QuitGameButton, QuitGameText);
}

void UPauseMenuWidget::OnHoveredButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}

#pragma region //////////////// RESUME /////////////////
void UPauseMenuWidget::OnClickedResumeButton()
{
}

void UPauseMenuWidget::OnHoveredResumeButton()
{
	OnHoveredButton(ResumeHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredResumeButton()
{
	OnHoveredButton(ResumeHoverAnim, false);
}
#pragma endregion 

#pragma region //////////////// LOAD GAME /////////////////
void UPauseMenuWidget::OnClickedLoadGameButton()
{
}

void UPauseMenuWidget::OnHoveredLoadGameButton()
{
	OnHoveredButton(LoadGameHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredLoadGameButton()
{
	OnHoveredButton(LoadGameHoverAnim, false);
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
	if (bUnhoverTextSettings == true) OnUnhoveredSettingsButton();
}

void UPauseMenuWidget::OnHoveredSettingsButton()
{
	OnHoveredButton(SettingsHoverAnim, true, bWasSettingsMenuWidgetSpawned);
}

void UPauseMenuWidget::OnUnhoveredSettingsButton()
{
	OnHoveredButton(SettingsHoverAnim, false, bWasSettingsMenuWidgetSpawned);
}

#pragma endregion 

#pragma region //////////////// RESTART /////////////////
void UPauseMenuWidget::OnClickedRestartButton()
{
}

void UPauseMenuWidget::OnHoveredRestartButton()
{
	OnHoveredButton(RestartHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredRestartButton()
{
	OnHoveredButton(RestartHoverAnim, false);
}
#pragma endregion 

#pragma region //////////////// QUIT GAME /////////////////
void UPauseMenuWidget::OnClickedQuitGameButton()
{
}

void UPauseMenuWidget::OnHoveredQuitGameButton()
{
	OnHoveredButton(QuitGameHoverAnim);
}

void UPauseMenuWidget::OnUnhoveredQuitGameButton()
{
	OnHoveredButton(QuitGameHoverAnim, false);
}
#pragma endregion 

bool UPauseMenuWidget::RemoveCurrentMenuWidgetsFromViewport()
{
	if (CurrentSpawnedMenuWidgets.Num() == 0) return true;

	TArray<UUserWidget*> Widgets;
	CurrentSpawnedMenuWidgets.GenerateKeyArray(Widgets);

	if (CurrentSpawnedMenuWidgets.Contains(Widgets.Last()))
	{
		TFunction<void(bool)>* FunctionPtr = CurrentSpawnedMenuWidgets.Find(Widgets.Last());
		(*FunctionPtr)(true);
		CurrentSpawnedMenuWidgets.Remove(Widgets.Last());
	}
	return false;
}

void UPauseMenuWidget::SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore)
{
	TArray<UButton*> AllMenuButtons;
	MenuButtonsAndText.GenerateKeyArray(AllMenuButtons);

	for (UButton* CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton) continue;

		UTextBlock* CurrentButtonText = *MenuButtonsAndText.Find(CurrentMenuButton);
		if (CurrentButtonText == nullptr) continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
		CurrentButtonText->SetColorAndOpacity(bEnable ? TextOriginalColor : TextDisabledColor);
	}
}