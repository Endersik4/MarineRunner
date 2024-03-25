// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PauseMenuWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;

struct FVisiblePauseMenu
{
	TObjectPtr<UUserWidget> VisibleWidget = nullptr;
	TFunction<void(bool)> FunctionToHideWidget = nullptr;

	FVisiblePauseMenu(TObjectPtr<UUserWidget> _VisibleWidget, TFunction<void(bool)> _FunctionToHideWidget)
	{
		VisibleWidget = _VisibleWidget;
		FunctionToHideWidget = _FunctionToHideWidget;
	}

	bool operator==(const TObjectPtr<UUserWidget>& CompareToVisibleWidget)
	{
		return VisibleWidget == CompareToVisibleWidget;
	}
};

UCLASS()
class MARINERUNNER_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> BackgroundBlurImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> MarineRunnerTitleImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> ResumeText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> ResumeButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> LoadGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> LoadGameButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> SettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> SettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> SaveGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> SaveGameButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> QuitGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> QuitGameButton = nullptr;

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
	bool BackToPreviousMenu();
	TArray<FVisiblePauseMenu> CurrentSpawnedMenuWidgets;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu Settings")
		TSubclassOf<UUserWidget> ConfirmLoadingSaveWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu Settings")
		TObjectPtr < USoundBase> PauseMenuMusic = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Load Game Menu")
		TSubclassOf<UUserWidget> LoadGameMenuWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Menu")
		TSubclassOf<UUserWidget> SettingsMenuWidgetClass = nullptr;

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);

	enum EMusicType MusicTypeBeforePause;
	void PlayPauseMenuMusic();
	void StopPauseMenuMusic();
	UPROPERTY(Transient)
		TObjectPtr<	class UAudioComponent> CurrentPauseMenuMusic = nullptr;

	UPROPERTY(Transient)
		bool bWasLoadGameMenuWidgetSpawned = false; 
	void SpawnLoadGameMenuWidget();
	void RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame = false);
	UPROPERTY(Transient)
		TObjectPtr<class ULoadGameMenuWidget> LoadGameMenuWidget = nullptr;

	UPROPERTY(Transient)
		bool bWasSettingsMenuWidgetSpawned = false;
	void SpawnSettingsMenuWidget();
	void RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings = false);
	UPROPERTY(Transient)
		TObjectPtr<class USettingsMenuWidget> SettingsMenuWidget = nullptr;

	void FillMenuButtonsAndTextMap();
	UPROPERTY(Transient)
		TArray< TObjectPtr<UButton>> AllMenuButtons;
	void SetEnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore = nullptr);

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<class UMarineRunnerGameInstance> GameInstance = nullptr;
};
