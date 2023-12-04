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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* NewGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* NewGameButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* LoadGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* LoadGameButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* SettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* SettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* QuitGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* QuitGameButton;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* ShowPauseWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ContinueHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* NewGameHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LoadGameHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* QuitGameHoverAnim = nullptr;
	

	UFUNCTION()
		void OnClickedContinueButton();
	UFUNCTION()
		void OnHoveredContinueButton();
	UFUNCTION()
		void OnUnhoveredContinueButton();

	UFUNCTION()
		void OnClickedNewGameButton();
	UFUNCTION()
		void OnHoveredNewGameButton();
	UFUNCTION()
		void OnUnhoveredNewGameButton();

	UFUNCTION()
		void OnClickedLoadGameButton();
	UFUNCTION()
		void OnHoveredLoadGameButton();
	UFUNCTION()
		void OnUnhoveredLoadGameButton();

	UFUNCTION()
		void OnClickedSettingsButton();
	UFUNCTION()
		void OnHoveredSettingsButton();
	UFUNCTION()
		void OnUnhoveredSettingsButton();

	UFUNCTION()
		void OnClickedQuitGameButton();
	UFUNCTION()
		void OnHoveredQuitGameButton();
	UFUNCTION()
		void OnUnhoveredQuitGameButton();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayMainMenuAnim();

	UFUNCTION(BlueprintImplementableEvent)
		void GetTextFilesFromSaves(TArray<FString>& Txt_Files);

	// returns true if there is no more left active menu widgets
	bool RemoveCurrentMenuWidgetsFromViewport();
	TMap<UUserWidget*, TFunction<void(bool)>> CurrentSpawnedMenuWidgets;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		FName NewGameLevelName = "DemoV2";
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
		USoundBase* PauseMenuMusic;
	UPROPERTY(EditDefaultsOnly, Category = "Menu Menu Settings")
		TSubclassOf<class UConfirmLoadingGameWidget> ConfirmLoadingSaveWidgetClass;
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
	void FillMenuButtonsAndTextMap();
	TArray<UButton*> AllMenuButtons;
	void SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore = nullptr);

	// Continue Game
	TArray<FString> SavesNamePath;

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);
	class UMarineRunnerGameInstance* MarineRunnerGameInstance;

};
