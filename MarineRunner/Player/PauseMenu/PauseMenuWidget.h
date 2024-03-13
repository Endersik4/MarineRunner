// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PauseMenuWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
UCLASS()
class MARINERUNNER_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> BackgroundBlurImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> MarineRunnerTitleImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> ResumeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> ResumeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> LoadGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> LoadGameButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> SettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> SettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> SaveGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> SaveGameButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> QuitGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> QuitGameButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> ShowPauseWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> ResumeHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> LoadGameHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> SettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> SaveGameHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> QuitGameHoverAnim = nullptr;

	UFUNCTION()
		void OnClickedResumeButton();
	UFUNCTION()
		void OnHoveredResumeButton();
	UFUNCTION()
		void OnUnhoveredResumeButton();

	UFUNCTION()
		void OnClickedSaveGameButton();
	UFUNCTION()
		void OnHoveredSaveGameButton();
	UFUNCTION()
		void OnUnhoveredSaveGameButton();

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
public:
	// returns true if there is no more left active menu widgets
	bool RemoveCurrentMenuWidgetsFromViewport();
	TMap< TObjectPtr<UUserWidget>, TFunction<void(bool)>> CurrentSpawnedMenuWidgets;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu Settings")
		TSubclassOf<UUserWidget> ConfirmLoadingSaveWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu Settings")
		TObjectPtr < USoundBase> PauseMenuMusic;
	UPROPERTY(EditDefaultsOnly, Category = "Load Game Menu")
		TSubclassOf<UUserWidget> LoadGameMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Menu")
		TSubclassOf<UUserWidget> SettingsMenuWidgetClass;

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);

	enum EMusicType MusicTypeBeforePause;
	void PlayPauseMenuMusic();
	void StopPauseMenuMusic();
	UPROPERTY(Transient)
		TObjectPtr<	class UAudioComponent> CurrentPauseMenuMusic;

	bool bWasLoadGameMenuWidgetSpawned;
	void SpawnLoadGameMenuWidget();
	void RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame = false);
	UPROPERTY(Transient)
		TObjectPtr<class ULoadGameMenuWidget> LoadGameMenuWidget;

	bool bWasSettingsMenuWidgetSpawned;
	void SpawnSettingsMenuWidget();
	void RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings = false);
	UPROPERTY(Transient)
		TObjectPtr<class USettingsMenuWidget> SettingsMenuWidget;

	void FillMenuButtonsAndTextMap();
	TArray< TObjectPtr<UButton>> AllMenuButtons;
	void SetEnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore = nullptr);

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;
	UPROPERTY(Transient)
		TObjectPtr<class UMarineRunnerGameInstance> GameInstance;
};
