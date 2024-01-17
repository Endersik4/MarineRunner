// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/SettingsMenuListEntry.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/InputKeySelector.h"
#include "Components/ListView.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/InputSettings.h"

#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "SettingsMenuWidget.h"

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
	SubSettingData = &ListEntryObject->MenuSettingsData;

	FunctionNameForCMD = SubSettingData->SubSettingFunctionName;

	HideAllUIElements();
	EnableEntry(SubSettingData->bEntryWidgetEnabled);

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
	if (SubSettingData->SubSettingType == EST_Category)
	{
		SubSettingType_Category();
		return;
	}

	SubSettingNameText->SetText(SubSettingData->SubSettingName);
	SubSettingNameText->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData->SubSettingType == EST_OnOff)  SubSettingType_OnOff();
	else if (SubSettingData->SubSettingType == EST_KeyMapping) SubSettingType_KeyBinding();
	else if (SubSettingData->SubSettingType == EST_SliderValue) SubSettingType_SliderValue();
	else SubSettingType_Quality();
}
#pragma endregion

#pragma region ////////////// SUBSETTING TYPE ////////////////
void USettingsMenuListEntry::SubSettingType_Category()
{
	CategoryNameText->SetText(SubSettingData->SubSettingName);
	CategoryNameText->SetColorAndOpacity(CategoryTextColor);

	CategoryNameText->SetVisibility(ESlateVisibility::Visible);
	HorizontalBoxForSettingsText->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuListEntry::SubSettingType_Quality()
{
	SubSettingQualityText->SetVisibility(ESlateVisibility::Visible);

	LeftArrowButton->SetVisibility(ESlateVisibility::Visible);
	RightArrowButton->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData->QualityCurrentValue == 0) LeftArrowButton->SetIsEnabled(false);
	if (SubSettingData->QualityCurrentValue == SubSettingData->QualityTypes.Num() - 1) RightArrowButton->SetIsEnabled(false);

	SubSettingQualityText->SetText(FText::FromString(SubSettingData->QualityTypes[SubSettingData->QualityCurrentValue]));

	AddValueToFunctionName(SubSettingData->QualityCurrentValue);
}

void USettingsMenuListEntry::SubSettingType_KeyBinding()
{
	SetProperKeyOnKeyMapText();

	KeyMappingButton->SetVisibility(ESlateVisibility::Visible);
	KeyMappingInputKeySelector->SetVisibility(ESlateVisibility::Visible);
	SubSettingQualityText->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuListEntry::SetProperKeyOnKeyMapText()
{
	FText KeyMap = FText::FromString("-empty-");

	if (UInputSettings::GetInputSettings()->DoesActionExist(FName(SubSettingData->KeyMappingName)))
	{
		KeyMap = FText::FromString(GetKeyActionName());
	}
	else if (UInputSettings::GetInputSettings()->DoesAxisExist(FName(SubSettingData->KeyMappingName)))
	{
		KeyMap = FText::FromString(GetKeyAxisName());
	}

	UInputSettings::GetInputSettings()->SaveKeyMappings();
	SubSettingQualityText->SetText(KeyMap);
}

void USettingsMenuListEntry::SubSettingType_SliderValue()
{
	SubSettingSlider->SetVisibility(ESlateVisibility::Visible);
	SliderButton->SetVisibility(ESlateVisibility::Visible);
	SubSettingSliderValueText->SetVisibility(ESlateVisibility::Visible);

	SubSettingSlider->SetMaxValue(SubSettingData->RangeOfSlider.GetUpperBoundValue());
	SubSettingSlider->SetMinValue(SubSettingData->RangeOfSlider.GetLowerBoundValue());
	SubSettingSlider->SetValue(SubSettingData->SliderCurrentValue);

	SetSubSettingSliderValueText(SubSettingData->SliderCurrentValue);

	AddValueToFunctionName(SubSettingData->SliderCurrentValue);
}

void USettingsMenuListEntry::SubSettingType_OnOff()
{
	SubSettingOnOffCheckBox->SetVisibility(ESlateVisibility::Visible);
	SubSettingOnOffButton->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData->bSettingEnabled) SubSettingOnOffCheckBox->SetCheckedState(ECheckBoxState::Checked);
	else SubSettingOnOffCheckBox->SetCheckedState(ECheckBoxState::Unchecked);

	AddValueToFunctionName(SubSettingData->bSettingEnabled ? 1 : 0);
}
#pragma endregion

#pragma region ///////// LEFT ARROW ////////////
void USettingsMenuListEntry::OnClickedLeftArrowButton()
{
	SubSettingData->QualityCurrentValue--;
	
	if (SubSettingData->QualityCurrentValue == 0) LeftArrowButton->SetIsEnabled(false);
	RightArrowButton->SetIsEnabled(true);

	SubSettingQualityText->SetText(FText::FromString(SubSettingData->QualityTypes[SubSettingData->QualityCurrentValue]));

	AddValueToFunctionName(SubSettingData->QualityCurrentValue);
}

void USettingsMenuListEntry::OnHoveredLeftArrowButton()
{
	PlayAnimatonForButton(LeftArrowHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredLeftArrowButton()
{
	PlayAnimatonForButton(LeftArrowHoverAnim, false);
}
#pragma endregion

#pragma region ///////// RIGHT ARROW ////////////
void USettingsMenuListEntry::OnClickedRightArrowButton()
{
	SubSettingData->QualityCurrentValue++;

	if (SubSettingData->QualityCurrentValue == SubSettingData->QualityTypes.Num() - 1) RightArrowButton->SetIsEnabled(false);
	LeftArrowButton->SetIsEnabled(true);

	SubSettingQualityText->SetText(FText::FromString(SubSettingData->QualityTypes[SubSettingData->QualityCurrentValue]));

	AddValueToFunctionName(SubSettingData->QualityCurrentValue);
}

void USettingsMenuListEntry::OnHoveredRightArrowButton()
{
	PlayAnimatonForButton(RightArrowHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredRightArrowButton()
{
	PlayAnimatonForButton(RightArrowHoverAnim, false);
}
#pragma endregion

#pragma region ///////// CHECK BOX ////////////
void USettingsMenuListEntry::OnClickedOnOffButton()
{
	SubSettingOnOffCheckBox->SetCheckedState(SubSettingData->bSettingEnabled ? ECheckBoxState::Unchecked : ECheckBoxState::Checked);
	SubSettingData->bSettingEnabled = SubSettingData->bSettingEnabled ? false : true;
	
	if (SubSettingData->bIsItObjectThatConnects == true)
	{
		FillConnectedSettingsEntryFromList();
		EnableAllConnectedSettingsEntry(SubSettingData->bSettingEnabled);
	}

	AddValueToFunctionName(SubSettingData->bSettingEnabled ? 1 : 0);
}

void USettingsMenuListEntry::OnHoveredOnOffButton()
{
	PlayAnimatonForButton(CheckBoxHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredOnOffButton()
{
	PlayAnimatonForButton(CheckBoxHoverAnim, false);
}

void USettingsMenuListEntry::FillConnectedSettingsEntryFromList()
{
	if (IsValid(ListEntryObject->SettingMenuWidget) == false) return;
	ConnectedSettingsEntryFromList.Empty();

	int32 IndexOfThisEntry = ListEntryObject->SettingMenuWidget->SettingsListView->GetIndexForItem(ListEntryObject);
	TArray<UObject*> Objects = ListEntryObject->SettingMenuWidget->SettingsListView->GetListItems();
	for (int i = IndexOfThisEntry+1; i != Objects.Num(); i++)
	{
		if (IsValid(Objects[i]) == false) continue;

		USettingsMenuListEntry* CurrentSettingEntryWidget = Cast<USettingsMenuListEntry>(ListEntryObject->SettingMenuWidget->SettingsListView->GetEntryWidgetFromItem(Objects[i]));
		if (IsValid(CurrentSettingEntryWidget) == false) continue;
		if (CurrentSettingEntryWidget->SubSettingData->bIsConnectedToOtherSettings == false)
			break;

		ConnectedSettingsEntryFromList.Add(CurrentSettingEntryWidget);
	}
}

void USettingsMenuListEntry::EnableAllConnectedSettingsEntry(bool bEnable)
{
	for (USettingsMenuListEntry* ConnectedEntry : ConnectedSettingsEntryFromList)
	{
		if (IsValid(ConnectedEntry) == false) continue;

		ConnectedEntry->EnableEntry(bEnable);
	}
}

#pragma endregion

#pragma region ///////// SLIDER ////////////
void USettingsMenuListEntry::OnValueChangedSlider(float Value)
{
	SetSubSettingSliderValueText(Value);

	SubSettingData->SliderCurrentValue = SubSettingData->DecimalNumbers == 0 ? (int)Value : Value;
	
	AddValueToFunctionName(Value);
}

void USettingsMenuListEntry::SetSubSettingSliderValueText(float Value)
{
	FString NewValue;
	if (SubSettingData->DecimalNumbers == 0)
	{
		NewValue = "-" + FString::FromInt((int)Value) + "-";
	}
	else
	{
		Value = FMath::RoundValuesToGivenDecimalNumbers(Value, SubSettingData->DecimalNumbers);
		NewValue = "-" + FString::SanitizeFloat(Value) + "-";
	}

	SubSettingSliderValueText->SetText(FText::FromString(NewValue));
}

void USettingsMenuListEntry::OnHoveredSliderButton()
{
	PlayAnimatonForButton(SliderHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredSliderButton()
{
	PlayAnimatonForButton(SliderHoverAnim, false);
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
	ReplaceKeyMap(SelectedKey);

	FText KeyMap = FText::FromString("-" + UKismetInputLibrary::Key_GetDisplayName(SelectedKey.Key, false).ToString() + "-");
	SubSettingQualityText->SetText(KeyMap);

	bIsWaitingForNewKey = false;
}

void USettingsMenuListEntry::ReplaceKeyMap(const FInputChord & KeyToReplaceFor)
{
	if (UInputSettings::GetInputSettings()->DoesActionExist(FName(SubSettingData->KeyMappingName)))
	{
		UInputSettings::GetInputSettings()->RemoveActionMapping(CurrentMappedActionKey);

		FInputActionKeyMapping NewActionKeyMapping = FInputActionKeyMapping(SubSettingData->KeyMappingName, KeyToReplaceFor.Key, KeyToReplaceFor.bShift, KeyToReplaceFor.bCtrl, KeyToReplaceFor.bAlt, KeyToReplaceFor.bCmd);
		UInputSettings::GetInputSettings()->AddActionMapping(NewActionKeyMapping, true);
		CurrentMappedActionKey = NewActionKeyMapping;
	}
	else if (UInputSettings::GetInputSettings()->DoesAxisExist(FName(SubSettingData->KeyMappingName)))
	{
		UInputSettings::GetInputSettings()->RemoveAxisMapping(CurrentMappedAxisKey);

		FInputAxisKeyMapping NewAxisKeyMapping = FInputAxisKeyMapping(SubSettingData->KeyMappingName, KeyToReplaceFor.Key, CurrentMappedAxisKey.Scale);
		UInputSettings::GetInputSettings()->AddAxisMapping(NewAxisKeyMapping, true);
		CurrentMappedAxisKey = NewAxisKeyMapping;
	}
	UInputSettings::GetInputSettings()->SaveKeyMappings();
}

void USettingsMenuListEntry::OnHoveredKeyMappingButton()
{
	if (bIsWaitingForNewKey == true) return;

	PlayAnimatonForButton(KeyMappingHoverAnim);
}
void USettingsMenuListEntry::OnUnhoveredKeyMappingButton()
{
	if (bIsWaitingForNewKey == true) return;

	PlayAnimatonForButton(KeyMappingHoverAnim, false);
}

FString USettingsMenuListEntry::GetKeyActionName()
{
	TArray<FInputActionKeyMapping> KeyActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(SubSettingData->KeyMappingName, KeyActionMappings);
	if (KeyActionMappings.Num() <= SubSettingData->IndexOfKey) 
		return "";

	CurrentMappedActionKey = KeyActionMappings[SubSettingData->IndexOfKey];
	FString CurrentMappedKeyName = "-" + UKismetInputLibrary::Key_GetDisplayName(KeyActionMappings[SubSettingData->IndexOfKey].Key, false).ToString() + "-";
	return CurrentMappedKeyName;
}

FString USettingsMenuListEntry::GetKeyAxisName()
{
	TArray<FInputAxisKeyMapping> KeyAxisMappings;
	UInputSettings::GetInputSettings()->GetAxisMappingByName(SubSettingData->KeyMappingName, KeyAxisMappings);
	if (KeyAxisMappings.Num() <= SubSettingData->IndexOfKey)
		return "";

	CurrentMappedAxisKey = KeyAxisMappings[SubSettingData->IndexOfKey];
	FString CurrentMappedKeyName = "-" + UKismetInputLibrary::Key_GetDisplayName(KeyAxisMappings[SubSettingData->IndexOfKey].Key, false).ToString() + "-";
	return CurrentMappedKeyName;
}

#pragma endregion

void USettingsMenuListEntry::AddValueToFunctionName(float Value)
{
	if (SubSettingData->SettingApplyType != ESAT_FunctionInCMD)
		return;

	FString ValueToStr = FString::SanitizeFloat(FMath::RoundValuesToGivenDecimalNumbers(Value, 3));
	ListEntryObject->FunctionNameToApply = FunctionNameForCMD + " " + ValueToStr;
}

void USettingsMenuListEntry::AddValueToFunctionName(int32 Value)
{
	if (SubSettingData->SettingApplyType != ESAT_FunctionInCMD)
		return;

	ListEntryObject->FunctionNameToApply = FunctionNameForCMD + " " + FString::FromInt(Value);
}

void USettingsMenuListEntry::AddValueToFunctionName(FString Value)
{
	if (SubSettingData->SettingApplyType != ESAT_FunctionInCMD)
		return;

	ListEntryObject->FunctionNameToApply = FunctionNameForCMD + " " + Value;
}

void USettingsMenuListEntry::PlayAnimatonForButton(UWidgetAnimation* AnimToPlay,bool bPlayForwardAnim)
{
	if (AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}

void USettingsMenuListEntry::EnableEntry(bool bEnable)
{
	SubSettingData->bEntryWidgetEnabled = bEnable;

	SubSettingNameText->SetIsEnabled(bEnable);
	CategoryNameText->SetIsEnabled(bEnable);

	SubSettingQualityText->SetIsEnabled(bEnable);

	LeftArrowButton->SetIsEnabled(bEnable);
	RightArrowButton->SetIsEnabled(bEnable);

	if (SubSettingData->QualityCurrentValue == 0) LeftArrowButton->SetIsEnabled(false);
	else if (SubSettingData->QualityCurrentValue == SubSettingData->QualityTypes.Num() - 1) RightArrowButton->SetIsEnabled(false);

	SubSettingOnOffCheckBox->SetIsEnabled(bEnable);
	SubSettingOnOffButton->SetIsEnabled(bEnable);

	SliderButton->SetIsEnabled(bEnable);
	SubSettingSlider->SetIsEnabled(bEnable);
	SubSettingSliderValueText->SetIsEnabled(bEnable);

	KeyMappingButton->SetIsEnabled(bEnable);
	KeyMappingInputKeySelector->SetIsEnabled(bEnable);
}