// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuData.h"

#include "SettingsMenuWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UButton;

struct FSettingSelectedStruct {
	UButton* SelectedButton;
	UWidgetAnimation* AnimationToPlay;

	FSettingSelectedStruct() {
		SelectedButton = nullptr;
		AnimationToPlay = nullptr;
	}

	FSettingSelectedStruct(UButton* NewSelectedButton, UWidgetAnimation* NewAnimationToPlay)
	{
		SelectedButton = NewSelectedButton;
		AnimationToPlay = NewAnimationToPlay;
	}
};

UCLASS()
class MARINERUNNER_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class USettingsMenuEntryObject> MenuSettingsDataObject;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UListView* SettingsListView;

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
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> GameSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> AudioSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> VideoSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> BindingsSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TSubclassOf<UUserWidget> SettingsAppliedWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float TimeToStartAnimationOnInitialized = 0.17f;

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, UButton* HoveredButton, bool bPlayForwardAnim = true);

	void FillCurrentMenuSettingsListView(const TArray<FMenuSettings> & DataToFillFrom);

	FSettingSelectedStruct CurrentSelectedSetting;

	void StartOnGameSettingsList();
	
	class USettingsMenuEntryObject* ConnectedSettings(USettingsMenuEntryObject* ObjectThatConnectOtherSettings, 	USettingsMenuEntryObject* ConstructedItemObject );

	void ActiveSettingByType(const FMenuSettings& SubSettingData, USettingsMenuEntryObject* SettingMenuObject);
	void ReplaceValueInGameInstanceByName(const FMenuSettings& SubSettingData);

	// Enable/Disable Menu Buttons
	void FillMenuButtonsAndTextMap();
	TMap<UButton*, UTextBlock*> MenuButtonsAndText;

	void LoadSavedSettingsToPlayer();

	void SpawnSettingsAcceptedWidget();

	class APlayerController* MarinePlayerController;
	class UMarineRunnerGameInstance* MarineGameInstance;
};
