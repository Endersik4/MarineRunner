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
		TObjectPtr<class UHorizontalBox> HorizontalBoxForSettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> SubSettingNameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> CategoryNameText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> SubSettingQualityText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> LeftArrowButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> RightArrowButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UCheckBox> SubSettingOnOffCheckBox = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> SubSettingOnOffButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> SliderButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class USlider> SubSettingSlider = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> SubSettingSliderValueText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> KeyMappingButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UInputKeySelector> KeyMappingInputKeySelector = nullptr;

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
		FLinearColor CategoryTextColor = FLinearColor::Blue;
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor WhenKeyMapColor = FLinearColor::Blue;

	UPROPERTY(Transient)
		FString FunctionNameToApplyInCMD = FString("");
	void AddValueToFunctionName(float Value);
	void AddValueToFunctionName(int32 Value);
	void AddValueToFunctionName(FString Value);

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true);
	void EnableEntry(bool bEnable);

	// Pointer to Entry Object SettingData
	FMenuSettings* SubSettingData = nullptr;

	// Key Mapping
	UPROPERTY(Transient)
		bool bIsWaitingForNewKey = false;
	UPROPERTY(Transient)
		FInputActionKeyMapping CurrentMappedActionKey = FInputActionKeyMapping();
	UPROPERTY(Transient)
		FInputAxisKeyMapping CurrentMappedAxisKey = FInputAxisKeyMapping();
	const FString GetKeyActionName();
	const FString GetKeyAxisName();
	UPROPERTY(Transient)
		FText PreviousKeyText = FText();

	// Hide everything in this widget
	void HideAllUIElements();

	// Connect to other sub settings
	UPROPERTY(Transient)
		TArray<TObjectPtr<USettingsMenuListEntry>> ConnectedSettingsEntryFromList = { nullptr };
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
		TObjectPtr<class USettingsMenuEntryObject> SettingEntryObject = nullptr;
};
