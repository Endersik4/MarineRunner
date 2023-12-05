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
		class UImage* MarineRunnerTitleImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ContinueText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* ContinueButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ContinueHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedContinueButton();
	UFUNCTION()
		void OnHoveredContinueButton();
	UFUNCTION()
		void OnUnhoveredContinueButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* NewGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* NewGameButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* NewGameHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedNewGameButton();
	UFUNCTION()
		void OnHoveredNewGameButton();
	UFUNCTION()
		void OnUnhoveredNewGameButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* LoadGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* LoadGameButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LoadGameHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedLoadGameButton();
	UFUNCTION()
		void OnHoveredLoadGameButton();
	UFUNCTION()
		void OnUnhoveredLoadGameButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* SettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* SettingsButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SettingsHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedSettingsButton();
	UFUNCTION()
		void OnHoveredSettingsButton();
	UFUNCTION()
		void OnUnhoveredSettingsButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* QuitGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* QuitGameButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* QuitGameHoverAnim = nullptr;
	UFUNCTION()
		void OnClickedQuitGameButton();
	UFUNCTION()
		void OnHoveredQuitGameButton();
	UFUNCTION()
		void OnUnhoveredQuitGameButton();


	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* ShowPauseWidgetAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* StartGameWidgetAnim = nullptr;

	// Play Show Main Menu Animation after delay
	UFUNCTION(BlueprintImplementableEvent)
		void PlayMainMenuAnim();

	// Returns all .json files from save game folder
	UFUNCTION(BlueprintImplementableEvent)
		void GetJsonFilesFromSaveFolder(TArray<FString>& Json_FilesPath);

	// returns true if there is no more left active menu widgets
	bool RemoveCurrentMenuWidgetsFromViewport();
	TMap<UUserWidget*, TFunction<void(bool)>> CurrentSpawnedMenuWidgets;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		FName NewGameLevelName = "DemoV2";
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		USoundBase* PauseMenuMusic;
	UPROPERTY(EditDefaultsOnly, Category = "Load Game Menu")
		TSubclassOf<class ULoadGameMenuWidget> LoadGameMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Menu")
		TSubclassOf<class USettingsMenuWidget> SettingsMenuWidgetClass;

	class UAudioComponent* CurrentPauseMenuMusic;

	// Load Game Widget
	bool bWasLoadGameMenuWidgetSpawned;
	void SpawnLoadGameMenuWidget();
	void RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame = false);
	class ULoadGameMenuWidget* LoadGameMenuWidget;

	// Settings Widget
	bool bWasSettingsMenuWidgetSpawned;
	void SpawnSettingsMenuWidget();
	void RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings = false);
	class USettingsMenuWidget* SettingsMenuWidget;

	// Enable/Disable Menu Buttons
	void AddAllMenuButtonsToArray();
	TArray<UButton*> AllMenuButtons;
	void SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore = nullptr);

	// Continue Game
	TArray<FString> Json_SaveFilesPath;
	struct FSaveDataMenuStruct GetLastSavedSaveGame();

	// Fade out when starting/loading game
	FTimerHandle StartGameHandle;
	void ContinueGame();
	void StartNewGame();
	void MainMenuFadeOut();

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);
	class UMarineRunnerGameInstance* MarineRunnerGameInstance;
	APlayerController* PlayerController;

};
