// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsInformationData.h"

#include "SettingsMenuWidget.generated.h"

class UTextBlock;
class UButton;

struct FSettingSelectedStruct {
	TObjectPtr<UButton> SelectedButton;
	TObjectPtr < UWidgetAnimation> AnimationToPlay;

	FSettingSelectedStruct() {
		SelectedButton = nullptr;
		AnimationToPlay = nullptr;
	}

	FSettingSelectedStruct(TObjectPtr < UButton> NewSelectedButton, TObjectPtr < UWidgetAnimation> NewAnimationToPlay)
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
		TObjectPtr<class UListView> SettingsListView;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> GameSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> GameSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> AudioSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> AudioSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> VideoSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> VideoSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> BindingsSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> BindingsSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> AcceptSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> AcceptSettingsButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> DefaultsSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> DefaultsSettingsButton;

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
		TArray<FMenuSettings> GameSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> AudioSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> VideoSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TArray<FMenuSettings> BindingsSettingsList;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Data")
		TSubclassOf<UUserWidget> SettingsAppliedWidgetClass;

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, TObjectPtr < UButton> HoveredButton, bool bPlayForwardAnim = true);

	void FillCurrentMenuSettingsListView(const TArray<FMenuSettings> & DataToFillFrom);

	FSettingSelectedStruct CurrentSelectedSetting;

	void StartOnGameSettingsList();
	
	TObjectPtr<class USettingsMenuEntryObject> ConnectedSettings(TObjectPtr < USettingsMenuEntryObject> ObjectThatConnectOtherSettings, TObjectPtr < USettingsMenuEntryObject> ConstructedItemObject );

	void ActiveSettingByType(const FMenuSettings& SubSettingData, TObjectPtr < USettingsMenuEntryObject> SettingMenuObject);
	void ReplaceValueInGameInstanceByName(const FMenuSettings& SubSettingData);

	// Enable/Disable Menu Buttons
	void FillMenuButtonsAndTextMap();
	TMap< TObjectPtr<UButton>, TObjectPtr<UTextBlock>> MenuButtonsAndText;

	void LoadSavedSettingsToPlayer();

	void SpawnSettingsAcceptedWidget();

	UPROPERTY(Transient)
		TObjectPtr<class APlayerController> MarinePlayerController;
	UPROPERTY(Transient)
		TObjectPtr<class UMarineRunnerGameInstance> MarineGameInstance;
};
