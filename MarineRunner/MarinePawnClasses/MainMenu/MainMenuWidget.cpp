// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/MainMenu/MainMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Animation/WidgetAnimation.h"

#include "MarineRunner/Widgets/Menu/SettingsMenuWidget.h"
#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/SaveGame/SaveGameJsonFile.h"

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
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	AddAllMenuButtonsToArray();

	MainMenuMusic();
	PlayMainMenuAnim();

	HideContinueButton();
}

void UMainMenuWidget::MainMenuMusic()
{
	MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(MarineRunnerGameInstance))
		MarineRunnerGameInstance->ChangeBackgroundMusic(EMT_PauseMusic);

	if (IsValid(PauseMenuMusic))
	{
		CurrentPauseMenuMusic = UGameplayStatics::SpawnSound2D(GetWorld(), PauseMenuMusic);
		if (IsValid(CurrentPauseMenuMusic))
			CurrentPauseMenuMusic->FadeIn(1.f);
	}
}

void UMainMenuWidget::HideContinueButton()
{
	GetJsonFilesFromSaveFolder(Json_SaveFilesPath);
	if (Json_SaveFilesPath.Num() > 0)
		return;

	ContinueButton->SetVisibility(ESlateVisibility::Hidden);
}

#pragma region ///////////////// CONTINUE ///////////////
void UMainMenuWidget::OnClickedContinueButton()
{
	MainMenuFadeOut();
	GetWorld()->GetTimerManager().SetTimer(StartGameHandle, this, &UMainMenuWidget::ContinueGame, StartGameWidgetAnim->GetEndTime(), false);
}

void UMainMenuWidget::ContinueGame()
{
	FSaveDataMenuStruct LastSavedSaveGame = GetLastSavedSaveGame();

	if (IsValid(MarineRunnerGameInstance))
	{
		MarineRunnerGameInstance->SlotSaveGameNameToLoad = LastSavedSaveGame.SaveName;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName(LastSavedSaveGame.LevelNameToLoad));
}

FSaveDataMenuStruct UMainMenuWidget::GetLastSavedSaveGame()
{
	FSaveDataMenuStruct LastSavedSaveGame;

	for (const FString& CurrJsonFilePath : Json_SaveFilesPath)
	{
		TSharedPtr<FJsonObject> JsonObject;
		bool bWasJsonDeserialized = USaveGameJsonFile::ReadJson(CurrJsonFilePath, JsonObject);
		if (bWasJsonDeserialized == false)
			continue;

		if (JsonObject->GetNumberField("SavedDataNumber") < LastSavedSaveGame.SaveNumber)
			continue;

			LastSavedSaveGame.SaveName = JsonObject->GetStringField("SavedDataName");
			LastSavedSaveGame.SaveNumber = JsonObject->GetNumberField("SavedDataNumber");
			LastSavedSaveGame.LevelNameToLoad = JsonObject->GetStringField("SavedLevelName");
	}

	return LastSavedSaveGame;
}

void UMainMenuWidget::OnHoveredContinueButton()
{
	PlayAnimatonForButton(ContinueHoverAnim);
}

void UMainMenuWidget::OnUnhoveredContinueButton()
{
	PlayAnimatonForButton(ContinueHoverAnim, false);
}
#pragma endregion

#pragma region ///////////////// NEW GAME ///////////////
void UMainMenuWidget::OnClickedNewGameButton()
{
	MainMenuFadeOut();

	GetWorld()->GetTimerManager().SetTimer(StartGameHandle, this, &UMainMenuWidget::StartNewGame, StartGameWidgetAnim->GetEndTime(), false);
}

void UMainMenuWidget::StartNewGame()
{
	// After opening new level the game will not load last saved save
	if (IsValid(MarineRunnerGameInstance))
		MarineRunnerGameInstance->bNewGame = true;

	UGameplayStatics::OpenLevel(GetWorld(), NewGameLevelName);
}

void UMainMenuWidget::OnHoveredNewGameButton()
{
	PlayAnimatonForButton(NewGameHoverAnim);
}

void UMainMenuWidget::OnUnhoveredNewGameButton()
{
	PlayAnimatonForButton(NewGameHoverAnim, false);
}
#pragma endregion

#pragma region ///////////////// LOAD GAME ///////////////
void UMainMenuWidget::OnClickedLoadGameButton()
{
	if (!bWasLoadGameMenuWidgetSpawned)
		SpawnLoadGameMenuWidget();
	else 
		RemoveLoadGameMenuWidgetFromViewport();
}

void UMainMenuWidget::SpawnLoadGameMenuWidget()
{
	if (!IsValid(PlayerController) || !LoadGameMenuWidgetClass)
		return;

	LoadGameMenuWidget = Cast<ULoadGameMenuWidget>(CreateWidget(PlayerController, LoadGameMenuWidgetClass));
	if (!IsValid(LoadGameMenuWidget))
		return;

	SetEnableAllMenuButtons(false, LoadGameButton);
	LoadGameMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(LoadGameMenuWidget, [this](bool bUnhoverText) { this->RemoveLoadGameMenuWidgetFromViewport(bUnhoverText); });

	bWasLoadGameMenuWidgetSpawned = true;
}

void UMainMenuWidget::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
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

void UMainMenuWidget::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, true, bWasLoadGameMenuWidgetSpawned);
}

void UMainMenuWidget::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false, bWasLoadGameMenuWidgetSpawned);
}
#pragma endregion

#pragma region ///////////////// SETTINGS ///////////////
void UMainMenuWidget::OnClickedSettingsButton()
{
	if (!bWasSettingsMenuWidgetSpawned)
		SpawnSettingsMenuWidget();
	else
		RemoveSettingsMenuWidgetFromViewport();
}

void UMainMenuWidget::SpawnSettingsMenuWidget()
{
	if (!IsValid(PlayerController) || !IsValid(SettingsMenuWidgetClass))
		return;

	SettingsMenuWidget = Cast<USettingsMenuWidget>(CreateWidget(PlayerController, SettingsMenuWidgetClass));
	if (!IsValid(SettingsMenuWidget)) 
		return;

	SetEnableAllMenuButtons(false, SettingsButton);
	SettingsMenuWidget->AddToViewport();

	CurrentSpawnedMenuWidgets.Add(SettingsMenuWidget, [this](bool bUnhoverText) { this->RemoveSettingsMenuWidgetFromViewport(bUnhoverText); });

	bWasSettingsMenuWidgetSpawned = true;
}

void UMainMenuWidget::RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings)
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

void UMainMenuWidget::OnHoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, true, bWasSettingsMenuWidgetSpawned);
}

void UMainMenuWidget::OnUnhoveredSettingsButton()
{
	PlayAnimatonForButton(SettingsHoverAnim, false, bWasSettingsMenuWidgetSpawned);
}
#pragma endregion

#pragma region ///////////////// QUIT GAME ///////////////

void UMainMenuWidget::OnClickedQuitGameButton()
{
	if (!IsValid(PlayerController))
		return;

	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}

void UMainMenuWidget::OnHoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim);
}

void UMainMenuWidget::OnUnhoveredQuitGameButton()
{
	PlayAnimatonForButton(QuitGameHoverAnim, false);
}
#pragma endregion

void UMainMenuWidget::MainMenuFadeOut()
{
	if (IsValid(CurrentPauseMenuMusic))
		CurrentPauseMenuMusic->FadeOut(StartGameWidgetAnim->GetEndTime(), 0.f);

	this->SetEnableAllMenuButtons(false);
	PlayAnimationForward(StartGameWidgetAnim);
}

bool UMainMenuWidget::RemoveCurrentMenuWidgetsFromViewport()
{
	if (CurrentSpawnedMenuWidgets.Num() == 0) 
		return true;

	TArray<TObjectPtr<UUserWidget>> SpawnedMenuWidgets;
	CurrentSpawnedMenuWidgets.GenerateKeyArray(SpawnedMenuWidgets);

	TFunction<void(bool)>* DeleteWidgetFunction = CurrentSpawnedMenuWidgets.Find(SpawnedMenuWidgets.Last());
	if (DeleteWidgetFunction)
	{
		(*DeleteWidgetFunction)(true);
		CurrentSpawnedMenuWidgets.Remove(SpawnedMenuWidgets.Last());
	}
	return false;
}

void UMainMenuWidget::AddAllMenuButtonsToArray()
{
	AllMenuButtons.Add(ContinueButton);
	AllMenuButtons.Add(NewGameButton);
	AllMenuButtons.Add(LoadGameButton);
	AllMenuButtons.Add(SettingsButton);
	AllMenuButtons.Add(QuitGameButton);
}

void UMainMenuWidget::SetEnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore)
{
	for (TObjectPtr<UButton> CurrentMenuButton : AllMenuButtons)
	{
		if (ButtonToIgnore == CurrentMenuButton) 
			continue;

		CurrentMenuButton->SetIsEnabled(bEnable);
	}
}

void UMainMenuWidget::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || !IsValid(AnimToPlay)) 
		return;

	if (bPlayForwardAnim) 
		PlayAnimationForward(AnimToPlay);
	else 
		PlayAnimationReverse(AnimToPlay);
}
