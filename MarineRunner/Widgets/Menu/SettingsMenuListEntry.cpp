// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/SettingsMenuListEntry.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/InputKeySelector.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/InputSettings.h"

#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"

void USettingsMenuListEntry::NativeConstruct()
{
	LeftArrowButton->OnClicked.AddDynamic(this, &USettingsMenuListEntry::OnClickedLeftArrowButton);
	LeftArrowButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredLeftArrowButton);
	LeftArrowButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredLeftArrowButton);

	RightArrowButton->OnClicked.AddDynamic(this, &USettingsMenuListEntry::OnClickedRightArrowButton);
	RightArrowButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredRightArrowButton);
	RightArrowButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredRightArrowButton);

	SubSettingOnOffButton->OnClicked.AddDynamic(this, &USettingsMenuListEntry::OnClickedOnOffButton);
	SubSettingOnOffButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredOnOffButton);
	SubSettingOnOffButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredOnOffButton);

	SubSettingSlider->OnValueChanged.AddDynamic(this, &USettingsMenuListEntry::OnValueChangedSlider);
	SliderButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredSliderButton);
	SliderButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredSliderButton);

	KeyMappingInputKeySelector->OnKeySelected.AddDynamic(this, &USettingsMenuListEntry::OnKeySelectedInputKeySelector);
	KeyMappingInputKeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &USettingsMenuListEntry::OnIsSelectingKeyChangedInputKeySelector);
	KeyMappingButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredKeyMappingButton);
	KeyMappingButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredKeyMappingButton);
}

void USettingsMenuListEntry::NativeOnInitialized()
{
}

#pragma region ///////// PREPARE WIDGET //////////
void USettingsMenuListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ListEntryObject = Cast<USettingsMenuEntryObject>(ListItemObject);
	HideAllUIElements();
	SubSettingData = ListEntryObject->MenuSettingsData;
	FunctionNameForCMD = SubSettingData.SubSettingFunctionName;

	DisplayProperUIElements();
}

void USettingsMenuListEntry::HideAllUIElements()
{
	HorizontalBoxForSettingsText->SetVisibility(ESlateVisibility::Hidden);
	CategoryNameText->SetVisibility(ESlateVisibility::Hidden);

	SubSettingNameText->SetVisibility(ESlateVisibility::Hidden);
	SubSettingQualityText->SetVisibility(ESlateVisibility::Hidden);

	LeftArrowButton->SetVisibility(ESlateVisibility::Hidden);
	RightArrowButton->SetVisibility(ESlateVisibility::Hidden);

	SubSettingOnOffCheckBox->SetVisibility(ESlateVisibility::Hidden);
	SubSettingOnOffButton->SetVisibility(ESlateVisibility::Hidden);

	KeyMappingButton->SetVisibility(ESlateVisibility::Hidden);
	KeyMappingInputKeySelector->SetVisibility(ESlateVisibility::Hidden);

	SubSettingSlider->SetVisibility(ESlateVisibility::Hidden);
	SliderButton->SetVisibility(ESlateVisibility::Hidden);
	SubSettingSliderValueText->SetVisibility(ESlateVisibility::Hidden);
}

void USettingsMenuListEntry::DisplayProperUIElements()
{
	if (SubSettingData.SubSettingType == EST_Category)
	{
		SubSettingType_Category();
		return;
	}

	SubSettingNameText->SetText(SubSettingData.SubSettingName);
	SubSettingNameText->SetVisibility(ESlateVisibility::Visible);
	SubSettingNameText->SetIsEnabled(true);

	if (SubSettingData.SubSettingType == EST_Quality) SubSettingType_Quality();
	else if (SubSettingData.SubSettingType == EST_KeyMapping) SubSettingType_KeyBinding();
	else if (SubSettingData.SubSettingType == EST_SliderValue) SubSettingType_SliderValue();
	else SubSettingType_OnOff();
}
#pragma endregion

#pragma region ////////////// SUBSETTING TYPE ////////////////
void USettingsMenuListEntry::SubSettingType_Category()
{
	CategoryNameText->SetText(SubSettingData.SubSettingName);
	CategoryNameText->SetColorAndOpacity(CategoryTextColor);

	CategoryNameText->SetVisibility(ESlateVisibility::Visible);
	HorizontalBoxForSettingsText->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuListEntry::SubSettingType_Quality()
{
	SubSettingQualityText->SetVisibility(ESlateVisibility::Visible);

	LeftArrowButton->SetVisibility(ESlateVisibility::Visible);
	RightArrowButton->SetVisibility(ESlateVisibility::Visible);

	SubSettingData.QualityCurrentValue = UKismetSystemLibrary::GetConsoleVariableIntValue(SubSettingData.SubSettingFunctionName);
	SubSettingQualityText->SetText(SubSettingData.QualityTypes[SubSettingData.QualityCurrentValue]);

	if (SubSettingData.QualityCurrentValue == 0) LeftArrowButton->SetIsEnabled(false);
	if (SubSettingData.QualityCurrentValue == SubSettingData.QualityTypes.Num() - 1) RightArrowButton->SetIsEnabled(false);

	MakeFunctionName(SubSettingData.QualityCurrentValue);
}

void USettingsMenuListEntry::SubSettingType_KeyBinding()
{
	TArray<FInputActionKeyMapping> KeyActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(SubSettingData.KeyMappingActionName, KeyActionMappings);
	FText KeyMap = FText::FromString("-empty-");
	if (KeyActionMappings.Num() > 0)
	{
		KeyMap = FText::FromString("-" + UKismetInputLibrary::Key_GetDisplayName(KeyActionMappings.Last().Key).ToString() + "-");
	}

	KeyMappingButton->SetVisibility(ESlateVisibility::Visible);
	KeyMappingInputKeySelector->SetVisibility(ESlateVisibility::Visible);
	SubSettingQualityText->SetVisibility(ESlateVisibility::Visible);
	SubSettingQualityText->SetText(KeyMap);
}

void USettingsMenuListEntry::SubSettingType_SliderValue()
{
	SubSettingSlider->SetVisibility(ESlateVisibility::Visible);
	SliderButton->SetVisibility(ESlateVisibility::Visible);
	SubSettingSliderValueText->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData.SettingApplyType == ESAT_MouseSens)
	{
		AMarinePlayerController* PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (IsValid(PC))
			SubSettingData.SliderCurrentValue = PC->GetMouseSensitivity();
	}

	SubSettingSlider->SetMaxValue(SubSettingData.RangeOfSlider.GetUpperBoundValue());
	SubSettingSlider->SetMinValue(SubSettingData.RangeOfSlider.GetLowerBoundValue());
	SubSettingSlider->SetValue(SubSettingData.SliderCurrentValue);

	SetSubSettingSliderValueText(SubSettingData.SliderCurrentValue);

	MakeFunctionName(SubSettingData.SliderCurrentValue);
}

void USettingsMenuListEntry::SubSettingType_OnOff()
{
	SubSettingOnOffCheckBox->SetVisibility(ESlateVisibility::Visible);
	SubSettingOnOffButton->SetVisibility(ESlateVisibility::Visible);

	SubSettingData.bSettingEnabled = UKismetSystemLibrary::GetConsoleVariableBoolValue(SubSettingData.SubSettingFunctionName);
	if (SubSettingData.bSettingEnabled) SubSettingOnOffCheckBox->SetCheckedState(ECheckBoxState::Checked);
	else SubSettingOnOffCheckBox->SetCheckedState(ECheckBoxState::Unchecked);

	MakeFunctionName(SubSettingData.bSettingEnabled ? 1 : 0);
}
#pragma endregion

#pragma region ///////// LEFT ARROW ////////////
void USettingsMenuListEntry::OnClickedLeftArrowButton()
{
	SubSettingData.QualityCurrentValue--;
	SubSettingQualityText->SetText(SubSettingData.QualityTypes[SubSettingData.QualityCurrentValue]);

	if (SubSettingData.QualityCurrentValue == 0) LeftArrowButton->SetIsEnabled(false);
	RightArrowButton->SetIsEnabled(true);

	MakeFunctionName(SubSettingData.QualityCurrentValue);
}

void USettingsMenuListEntry::OnHoveredLeftArrowButton()
{
	OnHoveredButton(LeftArrowHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredLeftArrowButton()
{
	OnHoveredButton(LeftArrowHoverAnim, false);
}
#pragma endregion

#pragma region ///////// RIGHT ARROW ////////////
void USettingsMenuListEntry::OnClickedRightArrowButton()
{
	SubSettingData.QualityCurrentValue++;
	SubSettingQualityText->SetText(SubSettingData.QualityTypes[SubSettingData.QualityCurrentValue]);

	if (SubSettingData.QualityCurrentValue == SubSettingData.QualityTypes.Num() - 1) RightArrowButton->SetIsEnabled(false);
	LeftArrowButton->SetIsEnabled(true);

	MakeFunctionName(SubSettingData.QualityCurrentValue);
}

void USettingsMenuListEntry::OnHoveredRightArrowButton()
{
	OnHoveredButton(RightArrowHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredRightArrowButton()
{
	OnHoveredButton(RightArrowHoverAnim, false);
}
#pragma endregion

#pragma region ///////// CHECK BOX ////////////
void USettingsMenuListEntry::OnClickedOnOffButton()
{
	SubSettingOnOffCheckBox->SetCheckedState(SubSettingData.bSettingEnabled ? ECheckBoxState::Unchecked : ECheckBoxState::Checked);
	SubSettingData.bSettingEnabled = SubSettingData.bSettingEnabled ? false : true;

	MakeFunctionName(SubSettingData.bSettingEnabled ? 1 : 0);
}

void USettingsMenuListEntry::OnHoveredOnOffButton()
{
	OnHoveredButton(CheckBoxHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredOnOffButton()
{
	OnHoveredButton(CheckBoxHoverAnim, false);
}
#pragma endregion

#pragma region ///////// SLIDER ////////////
void USettingsMenuListEntry::OnValueChangedSlider(float Value)
{
	SetSubSettingSliderValueText(Value);

	SubSettingData.SliderCurrentValue = Value;

	MakeFunctionName(Value);
}

void USettingsMenuListEntry::SetSubSettingSliderValueText(float Value)
{
	FString ConvertedValue = SubSettingData.DecimalNumbers == 0 ? FString::FromInt((int)Value) : UKismetTextLibrary::Conv_FloatToText(Value, ERoundingMode::HalfToEven, false, true, 1, 3, 1, SubSettingData.DecimalNumbers).ToString();
	FString NewValue = "-" + ConvertedValue + "-";
	SubSettingSliderValueText->SetText(FText::FromString(NewValue));
}
void USettingsMenuListEntry::OnHoveredSliderButton()
{
	OnHoveredButton(SliderHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredSliderButton()
{
	OnHoveredButton(SliderHoverAnim, false);
}

#pragma endregion

#pragma region //////// KEY MAPPING ////////////
void USettingsMenuListEntry::OnIsSelectingKeyChangedInputKeySelector()
{
	if (bIsWaitingForNewKey == true)
	{
		SubSettingQualityText->SetText(PreviousKeyText);
		bIsWaitingForNewKey = false;
		OnUnhoveredKeyMappingButton();
		return;
	}

	PreviousKeyText = SubSettingQualityText->GetText();
	SubSettingQualityText->SetText(FText::FromString("-press key-"));

	bIsWaitingForNewKey = true;
}

void USettingsMenuListEntry::OnKeySelectedInputKeySelector(FInputChord SelectedKey)
{
	TArray<FInputActionKeyMapping> KeyActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(SubSettingData.KeyMappingActionName, KeyActionMappings);
	for (int i = 0; i != KeyActionMappings.Num(); i++)
	{
		UInputSettings::GetInputSettings()->RemoveActionMapping(KeyActionMappings[0]);
	}

	FInputActionKeyMapping NewActionKeyMapping = FInputActionKeyMapping(SubSettingData.KeyMappingActionName, SelectedKey.Key, SelectedKey.bShift, SelectedKey.bCtrl, SelectedKey.bAlt, SelectedKey.bCmd);
	UInputSettings::GetInputSettings()->AddActionMapping(NewActionKeyMapping, true);

	FText KeyMap = FText::FromString("-" + UKismetInputLibrary::Key_GetDisplayName(SelectedKey.Key).ToString() + "-");
	SubSettingQualityText->SetText(KeyMap);

	bIsWaitingForNewKey = false;
}

void USettingsMenuListEntry::OnHoveredKeyMappingButton()
{
	if (bIsWaitingForNewKey == true) return;

	OnHoveredButton(KeyMappingHoverAnim);
}
void USettingsMenuListEntry::OnUnhoveredKeyMappingButton()
{
	if (bIsWaitingForNewKey == true) return;

	OnHoveredButton(KeyMappingHoverAnim, false);
}
#pragma endregion

void USettingsMenuListEntry::MakeFunctionName(float Value)
{
	if (SubSettingData.SettingApplyType != ESAT_FunctionInCMD)
	{
		if (ListEntryObject) ListEntryObject->MenuSettingsData = SubSettingData;
		return;
	}

	FString ValueToStr = UKismetTextLibrary::Conv_FloatToText(Value, ERoundingMode::HalfToEven, false, true, 1, 3, 1, 1).ToString();
	SubSettingData.SubSettingFunctionName = FunctionNameForCMD + " " + ValueToStr;
	if (ListEntryObject) ListEntryObject->MenuSettingsData = SubSettingData;
}

void USettingsMenuListEntry::MakeFunctionName(int32 Value)
{
	if (SubSettingData.SettingApplyType != ESAT_FunctionInCMD)
	{
		if (ListEntryObject) ListEntryObject->MenuSettingsData = SubSettingData;
		return;
	}

	SubSettingData.SubSettingFunctionName = FunctionNameForCMD + " " + FString::FromInt(Value);
	if (ListEntryObject) ListEntryObject->MenuSettingsData = SubSettingData;
}

void USettingsMenuListEntry::OnHoveredButton(UWidgetAnimation* AnimToPlay,bool bPlayForwardAnim)
{
	if (AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}