// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SettingsMenuEntryObject.h"

#include "SettingsMenuListEntry.generated.h"

UCLASS()
class MARINERUNNER_API USettingsMenuListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UHorizontalBox> HorizontalBoxForSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> SubSettingNameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> CategoryNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> SubSettingQualityText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> LeftArrowButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> RightArrowButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UCheckBox> SubSettingOnOffCheckBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> SubSettingOnOffButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> SliderButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class USlider> SubSettingSlider;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> SubSettingSliderValueText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> KeyMappingButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UInputKeySelector> KeyMappingInputKeySelector;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> LeftArrowHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> RightArrowHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> CheckBoxHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> SliderHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> KeyMappingHoverAnim = nullptr;

	UFUNCTION()
		void OnClickedLeftArrowButton();
	UFUNCTION()
		void OnHoveredLeftArrowButton();
	UFUNCTION()
		void OnUnhoveredLeftArrowButton();

	UFUNCTION()
		void OnClickedRightArrowButton();
	UFUNCTION()
		void OnHoveredRightArrowButton();
	UFUNCTION()
		void OnUnhoveredRightArrowButton();

	UFUNCTION()
		void OnClickedOnOffButton();
	UFUNCTION()
		void OnHoveredOnOffButton();
	UFUNCTION()
		void OnUnhoveredOnOffButton();

	UFUNCTION()
		void OnValueChangedSlider(float Value);
	UFUNCTION()
		void OnHoveredSliderButton();
	UFUNCTION()
		void OnUnhoveredSliderButton();

	UFUNCTION()
		void OnIsSelectingKeyChangedInputKeySelector();
	UFUNCTION()
		void OnKeySelectedInputKeySelector(FInputChord SelectedKey);
	UFUNCTION()
		void OnHoveredKeyMappingButton();
	UFUNCTION()
		void OnUnhoveredKeyMappingButton();


private:
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor CategoryTextColor;
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor WhenKeyMapColor;

	FString FunctionNameToApplyInCMD;
	void AddValueToFunctionName(float Value);
	void AddValueToFunctionName(int32 Value);
	void AddValueToFunctionName(FString Value);

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true);
	void EnableEntry(bool bEnable);

	// Pointer to Entry Object SettingData
	FMenuSettings* SubSettingData;

	// Key Mapping
	bool bIsWaitingForNewKey = false;
	FInputActionKeyMapping CurrentMappedActionKey;
	FInputAxisKeyMapping CurrentMappedAxisKey;
	FString GetKeyActionName();
	FString GetKeyAxisName();
	FText PreviousKeyText;

	// Hide everything in this widget
	void HideAllUIElements();

	// Connect to other sub settings
	TArray<TObjectPtr<USettingsMenuListEntry>> ConnectedSettingsEntryFromList;
	void FillConnectedSettingsEntryFromList();
	void EnableAllConnectedSettingsEntry(bool bEnable);

	void SetSubSettingSliderValueText(float Value);

	void DisplayProperUIElements();

	void SubSettingType_Category();
	void SubSettingType_Quality();

	void SubSettingType_KeyBinding();
	void SetProperKeyOnKeyMapText();
	void ReplaceKeyMap(const FInputChord& KeyToReplaceFor);

	void SubSettingType_SliderValue();
	void SubSettingType_OnOff();

	UPROPERTY(Transient)
		TObjectPtr<class USettingsMenuEntryObject> SettingEntryObject;
};
