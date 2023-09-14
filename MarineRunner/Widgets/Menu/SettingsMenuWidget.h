// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UButton;
UCLASS()
class MARINERUNNER_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* GameSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* GameSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* AudioSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* AudioSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* VideoSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* VideoSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* BindingsSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* BindingsSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* AcceptSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* AcceptSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* DefaultsSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* DefaultsSettingsButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* GameSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* AudioSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* VideoSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* BindingsSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* AcceptSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* DefaultsSettingsHoverAnim = nullptr;

	UFUNCTION()
		void OnClickedGameSettingsButton();
	UFUNCTION()
		void OnHoveredGameSettingsButton();
	UFUNCTION()
		void OnUnhoveredGameSettingsButton();

	UFUNCTION()
		void OnClickedAudioSettingsButton();
	UFUNCTION()
		void OnHoveredAudioSettingsButton();
	UFUNCTION()
		void OnUnhoveredAudioSettingsButton();

	UFUNCTION()
		void OnClickedVideoSettingsButton();
	UFUNCTION()
		void OnHoveredVideoSettingsButton();
	UFUNCTION()
		void OnUnhoveredVideoSettingsButton();

	UFUNCTION()
		void OnClickedBindingsSettingsButton();
	UFUNCTION()
		void OnHoveredBindingsSettingsButton();
	UFUNCTION()
		void OnUnhoveredBindingsSettingsButton();

	UFUNCTION()
		void OnClickedAcceptSettingsButton();
	UFUNCTION()
		void OnHoveredAcceptSettingsButton();
	UFUNCTION()
		void OnUnhoveredAcceptSettingsButton();

	UFUNCTION()
		void OnClickedDefaultSettingsButton();
	UFUNCTION()
		void OnHoveredDefaultSettingsButton();
	UFUNCTION()
		void OnUnhoveredDefaultSettingsButton();


private:
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor TextOriginalColor;
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor TextOnClickedColor;
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor TextDisabledColor;

	void OnHoveredButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);

	// Enable/Disable Menu Buttons
	void FillMenuButtonsAndTextMap();
	TMap<UButton*, UTextBlock*> MenuButtonsAndText;
	void SetEnableAllMenuButtons(bool bEnable, UButton* ButtonToIgnore = nullptr);
};
