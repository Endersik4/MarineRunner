// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"


class UTextBlock;
class UButton;
UCLASS()
class MARINERUNNER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> MarineRunnerTitleImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ContinueText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> ContinueButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ContinueHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedContinueButton();
	UFUNCTION()
		void OnHoveredContinueButton();
	UFUNCTION()
		void OnUnhoveredContinueButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> NewGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> NewGameButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> NewGameHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedNewGameButton();
	UFUNCTION()
		void OnHoveredNewGameButton();
	UFUNCTION()
		void OnUnhoveredNewGameButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> LoadGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> LoadGameButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> LoadGameHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedLoadGameButton();
	UFUNCTION()
		void OnHoveredLoadGameButton();
	UFUNCTION()
		void OnUnhoveredLoadGameButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> SettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> SettingsButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SettingsHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedSettingsButton();
	UFUNCTION()
		void OnHoveredSettingsButton();
	UFUNCTION()
		void OnUnhoveredSettingsButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> QuitGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> QuitGameButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> QuitGameHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedQuitGameButton();
	UFUNCTION()
		void OnHoveredQuitGameButton();
	UFUNCTION()
		void OnUnhoveredQuitGameButton();


	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ShowPauseWidgetAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> StartGameWidgetAnim = nullptr;

	// Play Show Main Menu Animation after delay
	UFUNCTION(BlueprintImplementableEvent)
		void PlayMainMenuAnim();

	// Returns all .json files from save game folder
	UFUNCTION(BlueprintImplementableEvent)
		void GetJsonFilesFromSaveFolder(TArray<FString>& Json_FilesPath);

	// returns true if there is no more left active menu widgets
	bool RemoveCurrentMenuWidgetsFromViewport();
	TMap< TObjectPtr<UUserWidget>, TFunction<void(bool)>> CurrentSpawnedMenuWidgets;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		FName NewGameLevelName = "L_GameInformationMap";
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		TObjectPtr<USoundBase> PauseMenuMusic;
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		TSubclassOf<class ULoadGameMenuWidget> LoadGameMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		TSubclassOf<class USettingsMenuWidget> SettingsMenuWidgetClass;

	UPROPERTY(Transient)
		TObjectPtr<class UAudioComponent> CurrentPauseMenuMusic;

	void MainMenuMusic();
	// If there are no saves then hide continue button
	void HideContinueButton();

	// Load Game Widget
	bool bWasLoadGameMenuWidgetSpawned;
	void SpawnLoadGameMenuWidget();
	void RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame = false);
	UPROPERTY(Transient)
		TObjectPtr<class ULoadGameMenuWidget> LoadGameMenuWidget;

	// Settings Widget
	bool bWasSettingsMenuWidgetSpawned;
	void SpawnSettingsMenuWidget();
	void RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings = false);
	UPROPERTY(Transient)
		TObjectPtr<class USettingsMenuWidget> SettingsMenuWidget;

	// Enable/Disable Menu Buttons
	void AddAllMenuButtonsToArray();
	TArray<TObjectPtr<UButton>> AllMenuButtons;
	void SetEnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore = nullptr);

	// Continue Game
	TArray<FString> Json_SaveFilesPath;
	struct FSaveDataMenuStruct GetLastSavedSaveGame();

	// Fade out when starting/loading game
	FTimerHandle StartGameHandle;
	void ContinueGame();
	void StartNewGame();
	void MainMenuFadeOut();

	void PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);
	UPROPERTY(Transient)
		TObjectPtr<class UMarineRunnerGameInstance> MarineRunnerGameInstance;
	UPROPERTY(Transient)
		TObjectPtr<APlayerController> PlayerController;

};
