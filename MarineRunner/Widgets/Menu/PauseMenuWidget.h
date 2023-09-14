// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UButton;
UCLASS()
class MARINERUNNER_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundBlurImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* MarineRunnerTitleImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ResumeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* ResumeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* LoadGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* LoadGameButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* SettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* SettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* RestartText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* RestartButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* QuitGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* QuitGameButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ResumeHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LoadGameHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* RestartHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* QuitGameHoverAnim = nullptr;

	UFUNCTION()
		void OnClickedResumeButton();
	UFUNCTION()
		void OnHoveredResumeButton();
	UFUNCTION()
		void OnUnhoveredResumeButton();

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
		void OnClickedRestartButton();
	UFUNCTION()
		void OnHoveredRestartButton();
	UFUNCTION()
		void OnUnhoveredRestartButton();

	UFUNCTION()
		void OnClickedQuitGameButton();
	UFUNCTION()
		void OnHoveredQuitGameButton();
	UFUNCTION()
		void OnUnhoveredQuitGameButton();

	// returns true if there is no more left active menu widgets
	bool RemoveCurrentMenuWidgetsFromViewport();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Settings Menu")
		TSubclassOf<UUserWidget> SettingsMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor TextOriginalColor;
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor TextOnClickedColor;
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor TextDisabledColor;

	TMap<UUserWidget*, TFunction<void(bool)>> CurrentSpawnedMenuWidgets;

	void OnHoveredButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);

	// Settings Widget
	bool bWasSettingsMenuWidgetSpawned;
	void SpawnSettingsMenuWidget();
	void RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings = false);
	class USettingsMenuWidget* SettingsMenuWidget;

	// Enable/Disable Menu Buttons
	void FillMenuButtonsAndTextMap();
	TMap<UButton*, UTextBlock*> MenuButtonsAndText;
	void SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore = nullptr);
};
