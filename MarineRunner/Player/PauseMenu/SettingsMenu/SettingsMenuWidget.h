// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsInformationData.h"

#include "SettingsMenuWidget.generated.h"

class UTextBlock;
class UButton;

struct FSettingSelected {
	TObjectPtr<UButton> SelectedButton;
	TObjectPtr < UWidgetAnimation> AnimationToPlay;

	FSettingSelected() {
		SelectedButton = nullptr;
		AnimationToPlay = nullptr;
	}

	FSettingSelected(TObjectPtr < UButton> NewSelectedButton, TObjectPtr < UWidgetAnimation> NewAnimationToPlay)
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
		TSubclassOf<class USettingsMenuEntryObject> MenuSettingsDataObject = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UListView> SettingsListView = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> GameSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> GameSettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> AudioSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> AudioSettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> VideoSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> VideoSettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> BindingsSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> BindingsSettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> AcceptSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> AcceptSettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> DefaultsSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> DefaultsSettingsButton = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> GameSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> AudioSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> VideoSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> BindingsSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> AcceptSettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> DefaultsSettingsHoverAnim = nullptr;

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
		TArray<FMenuSettings> GameSettingsList = {FMenuSettings()};
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> AudioSettingsList = { FMenuSettings() };
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> VideoSettingsList = { FMenuSettings() };
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> BindingsSettingsList = { FMenuSettings() };
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TSubclassOf<UUserWidget> SettingsAppliedWidgetClass = nullptr;

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, TObjectPtr < UButton> HoveredButton, bool bPlayForwardAnim = true);

	void FillCurrentMenuSettingsListView(const TArray<FMenuSettings> & DataToFillFrom);

	FSettingSelected CurrentSelectedSetting;

	void StartOnGameSettingsList();
	
	TObjectPtr<class USettingsMenuEntryObject> ConnectedSettings(TObjectPtr < USettingsMenuEntryObject> ObjectThatConnectOtherSettings, TObjectPtr < USettingsMenuEntryObject> ConstructedItemObject );

	void ActiveSettingByType(const FMenuSettings& SubSettingData, TObjectPtr < USettingsMenuEntryObject> SettingMenuObject);
	void ReplaceValueInGameInstanceByName(const FMenuSettings& SubSettingData);

	// Enable/Disable Menu Buttons
	void FillMenuButtonsAndTextMap();
	UPROPERTY(Transient)
		TMap< TObjectPtr<UButton>, TObjectPtr<UTextBlock>> MenuButtonsAndText = { {nullptr, nullptr} };

	void LoadSavedSettingsToPlayer();

	void SpawnSettingsAcceptedWidget();

	UPROPERTY(Transient)
		TObjectPtr<class APlayerController> MarinePlayerController = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<class UMarineRunnerGameInstance> MarineGameInstance = nullptr;
};
