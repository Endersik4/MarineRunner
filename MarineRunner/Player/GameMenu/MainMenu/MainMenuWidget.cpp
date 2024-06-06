// Copyright Adam Bartela.All Rights Reserved


#include "MainMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/AudioComponent.h"

#include "MainMenuPawn.h"
#include "MarineRunner/Player/GameMenu/SettingsMenu/SettingsMenuWidget.h"
#include "MarineRunner/Player/GameMenu/LoadGameMenu/LoadGameMenuWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Player/SaveLoadGame/JsonFileActions.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedContinueButton);
	ContinueButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredContinueButton);
	ContinueButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredContinueButton);

	NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickedNewGameButton);
	NewGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnHoveredNewGameButton);
	NewGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnUnhoveredNewGameButton);

	FWidgetAnimationDynamicEvent ShowWidgetAnimEvent;
	ShowWidgetAnimEvent.BindDynamic(this, &UMainMenuWidget::OnAnimFinished_FadeInWidget);
	BindToAnimationFinished(FadeInWidgetAnim, ShowWidgetAnimEvent);
}

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayAnimationForward(FadeInWidgetAnim);

	AllMenuButtons.Add(ContinueButton);
	AllMenuButtons.Add(NewGameButton);

	MainMenuMusic();
}

void UMainMenuWidget::OnAnimFinished_FadeInWidget()
{
	PlayAnimationForward(ShowPauseWidgetAnim);
	HideContinueButton();
}

void UMainMenuWidget::MainMenuMusic()
{
	if (IsValid(GameInstance))
		GameInstance->ChangeBackgroundMusic(EMT_PauseMusic);

	if (IsValid(PauseMenuMusic))
	{
		CurrentMenuMusic = UGameplayStatics::SpawnSound2D(GetWorld(), PauseMenuMusic);
		if (IsValid(CurrentMenuMusic))
			CurrentMenuMusic->FadeIn(MainMenuMusicFadeIn);
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
	PlayAnimationForward(FadeOutWidgetAnim);
	MainMusicFadeOut();
	GetWorld()->GetTimerManager().SetTimer(StartGameHandle, this, &UMainMenuWidget::ContinueGame, FadeOutWidgetAnim->GetEndTime(), false);
}

void UMainMenuWidget::ContinueGame()
{
	FSaveDataMenuStruct LastSavedSaveGame = GetLastSavedSaveGame();
	if (IsValid(GameInstance))
	{
		GameInstance->SlotSaveGameNameToLoad = LastSavedSaveGame.SaveName;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName(LastSavedSaveGame.LevelNameToLoad));
}

FSaveDataMenuStruct UMainMenuWidget::GetLastSavedSaveGame()
{
	FSaveDataMenuStruct LastSavedSaveGame;

	for (const FString& CurrJsonFilePath : Json_SaveFilesPath)
	{
		TSharedPtr<FJsonObject> JsonObject;
		const bool bWasJsonDeserialized = USaveGameJsonFile::ReadJson(CurrJsonFilePath, JsonObject);
		if (!bWasJsonDeserialized)
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
	PlayAnimationForward(HidePauseWidgetAnim);
	this->EnableAllMenuButtons(false);

	FTimerDelegate SpawnZTGKInformationDelegate = FTimerDelegate::CreateUObject(this, &UMainMenuWidget::ShowZTGKInformation, true);
	GetWorld()->GetTimerManager().SetTimer(SpawnInformationHandle, SpawnZTGKInformationDelegate, HidePauseWidgetAnim->GetEndTime(), false);
}

void UMainMenuWidget::ShowZTGKInformation(bool bShow)
{
	if (bShow)
	{
		PlayAnimationForward(ShowZTGKInformationAnim);
		
		if (IsValid(MainMenuPawn))
		{
			MainMenuPawn->SetCanPressAnyKeyToContinue(true);
		}
	}
	else
	{
		PlayAnimationForward(HideZTGKInformationAnim);
	}
}

void UMainMenuWidget::StartNewGame()
{
	// After opening new level the game will not load last saved save
	if (IsValid(GameInstance))
		GameInstance->bNewGame = true;

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

void UMainMenuWidget::OnClickedLoadGameButton()
{
	Super::OnClickedLoadGameButton();

	if (bWasLoadGameMenuWidgetSpawned)
	{
		HideContinueButton();
		RemoveLoadGameMenuWidgetFromViewport();
	}
	else
	{
		SpawnLoadGameMenuWidget();
	}
}

void UMainMenuWidget::RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame)
{
	Super::RemoveLoadGameMenuWidgetFromViewport(bUnhoverTextLoadGame);

	HideContinueButton();
}

#pragma region ///////////////// QUIT GAME ///////////////

void UMainMenuWidget::OnClickedQuitGameButton()
{
	Super::OnClickedQuitGameButton();

	if (!IsValid(PlayerController))
		return;

	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}
#pragma endregion

void UMainMenuWidget::MainMusicFadeOut()
{
	if (IsValid(CurrentMenuMusic))
		CurrentMenuMusic->FadeOut(MainMenuMusicFadeOut, 0.f);

}
