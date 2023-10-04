// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MarineRunner/Widgets/Menu/SettingsMenuEntryObject.h"

#include "SettingsMenuListEntry.generated.h"

/**
 * 
 */
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
		class UHorizontalBox* HorizontalBoxForSettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* SubSettingNameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CategoryNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* SubSettingQualityText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* LeftArrowButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* RightArrowButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UCheckBox* SubSettingOnOffCheckBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* SubSettingOnOffButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* SliderButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class USlider* SubSettingSlider;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* SubSettingSliderValueText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* KeyMappingButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UInputKeySelector* KeyMappingInputKeySelector;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LeftArrowHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* RightArrowHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CheckBoxHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SliderHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* KeyMappingHoverAnim = nullptr;

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

	FString FunctionNameForCMD;
	void AddValueToFunctionName(float Value);
	void AddValueToFunctionName(int32 Value);

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true);
	void EnableEntry(bool bEnable);

	// Pointer to Entry Object SettingData
	FMenuSettings* SubSettingData;

	// Key Mapping
	FInputActionKeyMapping CurrentMappedActionKey;
	FInputAxisKeyMapping CurrentMappedAxisKey;
	FString GetKeyActionName();
	FString GetKeyAxisName();
	bool bIsWaitingForNewKey = false;
	FText PreviousKeyText;

	// Hide everything in this widget
	void HideAllUIElements();

	// Connect to other sub settings
	TArray<USettingsMenuListEntry*> ConnectedSettingsEntryFromList;
	void FillConnectedSettingsEntryFromList();
	void EnableAllConnectedSettingsEntry(bool bEnable);

	// Slider
	void SetSubSettingSliderValueText(float Value);
	float GetCurrentMouseSensitivity();

	void DisplayProperUIElements();

	void SubSettingType_Category();
	void SubSettingType_Quality();

	void SubSettingType_KeyBinding();
	void SetProperKeyOnKeyMapText();
	void ReplaceKeyMap(const FInputChord& KeyToReplaceFor);

	void SubSettingType_SliderValue();
	void SubSettingType_OnOff();

	class USettingsMenuEntryObject* ListEntryObject;
};
