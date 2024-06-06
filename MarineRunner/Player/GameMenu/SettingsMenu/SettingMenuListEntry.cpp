// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/GameMenu/SettingsMenu/SettingMenuListEntry.h"

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

#include "MarineRunner/Player/MarinePlayer.h"
#include "SettingsMenuWidget.h"

void USettingMenuListEntry::NativeConstruct()
{
	LeftArrowButton->OnClicked.AddDynamic(this, &USettingMenuListEntry::OnClickedLeftArrowButton);
	LeftArrowButton->OnHovered.AddDynamic(this, &USettingMenuListEntry::OnHoveredLeftArrowButton);
	LeftArrowButton->OnUnhovered.AddDynamic(this, &USettingMenuListEntry::OnUnhoveredLeftArrowButton);

	RightArrowButton->OnClicked.AddDynamic(this, &USettingMenuListEntry::OnClickedRightArrowButton);
	RightArrowButton->OnHovered.AddDynamic(this, &USettingMenuListEntry::OnHoveredRightArrowButton);
	RightArrowButton->OnUnhovered.AddDynamic(this, &USettingMenuListEntry::OnUnhoveredRightArrowButton);

	SubSettingOnOffButton->OnClicked.AddDynamic(this, &USettingMenuListEntry::OnClickedOnOffButton);
	SubSettingOnOffButton->OnHovered.AddDynamic(this, &USettingMenuListEntry::OnHoveredOnOffButton);
	SubSettingOnOffButton->OnUnhovered.AddDynamic(this, &USettingMenuListEntry::OnUnhoveredOnOffButton);

	SubSettingSlider->OnValueChanged.AddDynamic(this, &USettingMenuListEntry::OnValueChangedSlider);
	SliderButton->OnHovered.AddDynamic(this, &USettingMenuListEntry::OnHoveredSliderButton);
	SliderButton->OnUnhovered.AddDynamic(this, &USettingMenuListEntry::OnUnhoveredSliderButton);

	KeyMappingInputKeySelector->OnKeySelected.AddDynamic(this, &USettingMenuListEntry::OnKeySelectedInputKeySelector);
	KeyMappingInputKeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &USettingMenuListEntry::OnIsSelectingKeyChangedInputKeySelector);
	KeyMappingButton->OnHovered.AddDynamic(this, &USettingMenuListEntry::OnHoveredKeyMappingButton);
	KeyMappingButton->OnUnhovered.AddDynamic(this, &USettingMenuListEntry::OnUnhoveredKeyMappingButton);
}

#pragma region ///////// PREPARE WIDGET //////////
void USettingMenuListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!IsValid(ListItemObject))
		return;

	SettingEntryObject = Cast<USettingMenuEntryObject>(ListItemObject);
	if (!IsValid(SettingEntryObject))
		return;

	SubSettingData = &SettingEntryObject->MenuSettingsData;

	FunctionNameToApplyInCMD = SubSettingData->SubSettingFunctionName;

	HideAllUIElements();
	EnableEntry(SubSettingData->bEntryWidgetEnabled);

	DisplayProperUIElements();
}

void USettingMenuListEntry::HideAllUIElements()
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

void USettingMenuListEntry::DisplayProperUIElements()
{
	if (SubSettingData->SubSettingType == EST_Category)
	{
		SubSettingType_Category();
		return;
	}

	SubSettingNameText->SetText(SubSettingData->SubSettingName);
	SubSettingNameText->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData->SubSettingType == EST_OnOff)
	{
		SubSettingType_OnOff();
	}
	else if (SubSettingData->SubSettingType == EST_KeyMapping)
	{
		SubSettingType_KeyBinding();
	}
	else if (SubSettingData->SubSettingType == EST_SliderValue)
	{
		SubSettingType_SliderValue();
	}
	else
	{
		SubSettingType_Quality();
	}
}
#pragma endregion

#pragma region ////////////// SUBSETTING TYPE ////////////////
void USettingMenuListEntry::SubSettingType_Category()
{
	CategoryNameText->SetText(SubSettingData->SubSettingName);
	CategoryNameText->SetColorAndOpacity(CategoryTextColor);

	CategoryNameText->SetVisibility(ESlateVisibility::Visible);
	HorizontalBoxForSettingsText->SetVisibility(ESlateVisibility::Visible);
}

void USettingMenuListEntry::SubSettingType_Quality()
{
	SubSettingQualityText->SetVisibility(ESlateVisibility::Visible);

	LeftArrowButton->SetVisibility(ESlateVisibility::Visible);
	RightArrowButton->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData->QualityCurrentValue == 0)
	{
		LeftArrowButton->SetIsEnabled(false);
	}
	if (SubSettingData->QualityCurrentValue == SubSettingData->QualityTypes.Num() - 1)
	{
		RightArrowButton->SetIsEnabled(false);
	}

	SubSettingQualityText->SetText(FText::FromString(SubSettingData->QualityTypes[SubSettingData->QualityCurrentValue]));

	AddValueToFunctionName(SubSettingData->QualityCurrentValue);
}

void USettingMenuListEntry::SubSettingType_KeyBinding()
{
	SetProperKeyOnKeyMapText();

	KeyMappingButton->SetVisibility(ESlateVisibility::Visible);
	KeyMappingInputKeySelector->SetVisibility(ESlateVisibility::Visible);
	SubSettingQualityText->SetVisibility(ESlateVisibility::Visible);
}

void USettingMenuListEntry::SetProperKeyOnKeyMapText()
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

void USettingMenuListEntry::SubSettingType_SliderValue()
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

void USettingMenuListEntry::SubSettingType_OnOff()
{
	SubSettingOnOffCheckBox->SetVisibility(ESlateVisibility::Visible);
	SubSettingOnOffButton->SetVisibility(ESlateVisibility::Visible);

	if (SubSettingData->bSettingEnabled)
	{
		SubSettingOnOffCheckBox->SetCheckedState(ECheckBoxState::Checked);
	}
	else
	{
		SubSettingOnOffCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	}

	AddValueToFunctionName(SubSettingData->bSettingEnabled ? 1 : 0);
}
#pragma endregion

#pragma region ///////// LEFT ARROW ////////////
void USettingMenuListEntry::OnClickedLeftArrowButton()
{
	SubSettingData->QualityCurrentValue--;

	if (SubSettingData->QualityCurrentValue == 0)
		LeftArrowButton->SetIsEnabled(false);

	RightArrowButton->SetIsEnabled(true);

	SubSettingQualityText->SetText(FText::FromString(SubSettingData->QualityTypes[SubSettingData->QualityCurrentValue]));

	AddValueToFunctionName(SubSettingData->QualityCurrentValue);
}

void USettingMenuListEntry::OnHoveredLeftArrowButton()
{
	PlayAnimatonForButton(LeftArrowHoverAnim);
}

void USettingMenuListEntry::OnUnhoveredLeftArrowButton()
{
	PlayAnimatonForButton(LeftArrowHoverAnim, false);
}
#pragma endregion

#pragma region ///////// RIGHT ARROW ////////////
void USettingMenuListEntry::OnClickedRightArrowButton()
{
	SubSettingData->QualityCurrentValue++;

	if (SubSettingData->QualityCurrentValue == SubSettingData->QualityTypes.Num() - 1)
		RightArrowButton->SetIsEnabled(false);

	LeftArrowButton->SetIsEnabled(true);

	SubSettingQualityText->SetText(FText::FromString(SubSettingData->QualityTypes[SubSettingData->QualityCurrentValue]));

	AddValueToFunctionName(SubSettingData->QualityCurrentValue);
}

void USettingMenuListEntry::OnHoveredRightArrowButton()
{
	PlayAnimatonForButton(RightArrowHoverAnim);
}

void USettingMenuListEntry::OnUnhoveredRightArrowButton()
{
	PlayAnimatonForButton(RightArrowHoverAnim, false);
}
#pragma endregion

#pragma region ///////// CHECK BOX ////////////
void USettingMenuListEntry::OnClickedOnOffButton()
{
	SubSettingOnOffCheckBox->SetCheckedState(SubSettingData->bSettingEnabled ? ECheckBoxState::Unchecked : ECheckBoxState::Checked);
	SubSettingData->bSettingEnabled = SubSettingData->bSettingEnabled ? false : true;

	if (SubSettingData->bIsItObjectThatConnects)
	{
		FillConnectedSettingsEntryFromList();
		EnableAllConnectedSettingsEntry(SubSettingData->bSettingEnabled);
	}

	AddValueToFunctionName(SubSettingData->bSettingEnabled ? 1 : 0);
}

void USettingMenuListEntry::OnHoveredOnOffButton()
{
	PlayAnimatonForButton(CheckBoxHoverAnim);
}

void USettingMenuListEntry::OnUnhoveredOnOffButton()
{
	PlayAnimatonForButton(CheckBoxHoverAnim, false);
}

void USettingMenuListEntry::FillConnectedSettingsEntryFromList()
{
	if (!IsValid(SettingEntryObject->SettingMenuWidget))
		return;

	ConnectedSettingsEntryFromList.Empty();

	const int32& IndexOfThisEntry = SettingEntryObject->SettingMenuWidget->SettingsListView->GetIndexForItem(SettingEntryObject);
	TArray<UObject*> Objects = SettingEntryObject->SettingMenuWidget->SettingsListView->GetListItems();
	for (int i = IndexOfThisEntry + 1; i != Objects.Num(); i++)
	{
		if (!IsValid(Objects[i]))
			continue;

		const TObjectPtr<USettingMenuListEntry> CurrentSettingEntryWidget = Cast<USettingMenuListEntry>(SettingEntryObject->SettingMenuWidget->SettingsListView->GetEntryWidgetFromItem(Objects[i]));
		if (!IsValid(CurrentSettingEntryWidget))
			continue;
		if (!CurrentSettingEntryWidget->SubSettingData->bIsConnectedToOtherSettings)
			break;

		ConnectedSettingsEntryFromList.Add(CurrentSettingEntryWidget);
	}
}

void USettingMenuListEntry::EnableAllConnectedSettingsEntry(bool bEnable)
{
	for (TObjectPtr<USettingMenuListEntry> ConnectedEntry : ConnectedSettingsEntryFromList)
	{
		if (!IsValid(ConnectedEntry))
			continue;

		ConnectedEntry->EnableEntry(bEnable);
	}
}

#pragma endregion

#pragma region ///////// SLIDER ////////////
void USettingMenuListEntry::OnValueChangedSlider(float Value)
{
	SetSubSettingSliderValueText(Value);

	SubSettingData->SliderCurrentValue = SubSettingData->DecimalNumbers == 0 ? (int)Value : Value;

	AddValueToFunctionName(Value);
}

void USettingMenuListEntry::SetSubSettingSliderValueText(float Value)
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

void USettingMenuListEntry::OnHoveredSliderButton()
{
	PlayAnimatonForButton(SliderHoverAnim);
}

void USettingMenuListEntry::OnUnhoveredSliderButton()
{
	PlayAnimatonForButton(SliderHoverAnim, false);
}
#pragma endregion

#pragma region //////// KEY MAPPING ////////////
void USettingMenuListEntry::OnIsSelectingKeyChangedInputKeySelector()
{
	if (bIsWaitingForNewKey)
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


void USettingMenuListEntry::OnKeySelectedInputKeySelector(FInputChord SelectedKey)
{
	ReplaceKeyMap(SelectedKey);

	const FText& KeyMap = FText::FromString("-" + UKismetInputLibrary::Key_GetDisplayName(SelectedKey.Key, false).ToString() + "-");
	SubSettingQualityText->SetText(KeyMap);

	bIsWaitingForNewKey = false;
}

void USettingMenuListEntry::ReplaceKeyMap(const FInputChord& KeyToReplaceFor)
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

void USettingMenuListEntry::OnHoveredKeyMappingButton()
{
	if (bIsWaitingForNewKey)
		return;

	PlayAnimatonForButton(KeyMappingHoverAnim);
}
void USettingMenuListEntry::OnUnhoveredKeyMappingButton()
{
	if (bIsWaitingForNewKey)
		return;

	PlayAnimatonForButton(KeyMappingHoverAnim, false);
}

const FString USettingMenuListEntry::GetKeyActionName()
{
	TArray<FInputActionKeyMapping> KeyActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(SubSettingData->KeyMappingName, KeyActionMappings);
	if (KeyActionMappings.Num() <= SubSettingData->IndexOfKey)
		return "";

	CurrentMappedActionKey = KeyActionMappings[SubSettingData->IndexOfKey];
	const FString& CurrentMappedKeyName = "-" + UKismetInputLibrary::Key_GetDisplayName(KeyActionMappings[SubSettingData->IndexOfKey].Key, false).ToString() + "-";
	return CurrentMappedKeyName;
}

const FString USettingMenuListEntry::GetKeyAxisName()
{
	TArray<FInputAxisKeyMapping> KeyAxisMappings;
	UInputSettings::GetInputSettings()->GetAxisMappingByName(SubSettingData->KeyMappingName, KeyAxisMappings);
	if (KeyAxisMappings.Num() <= SubSettingData->IndexOfKey)
		return "-empty-";

	CurrentMappedAxisKey = KeyAxisMappings[SubSettingData->IndexOfKey];
	const FString& CurrentMappedKeyName = "-" + UKismetInputLibrary::Key_GetDisplayName(KeyAxisMappings[SubSettingData->IndexOfKey].Key, false).ToString() + "-";
	return CurrentMappedKeyName;
}

#pragma endregion

void USettingMenuListEntry::AddValueToFunctionName(float Value)
{
	if (SubSettingData->SettingApplyType != ESAT_FunctionInCMD)
		return;

	const FString& ValueToStr = FString::SanitizeFloat(FMath::RoundValuesToGivenDecimalNumbers(Value, 3));
	SettingEntryObject->FunctionNameToApply = FunctionNameToApplyInCMD + " " + ValueToStr;
}

void USettingMenuListEntry::AddValueToFunctionName(int32 Value)
{
	if (SubSettingData->SettingApplyType != ESAT_FunctionInCMD)
		return;

	SettingEntryObject->FunctionNameToApply = FunctionNameToApplyInCMD + " " + FString::FromInt(Value);
}

void USettingMenuListEntry::AddValueToFunctionName(FString Value)
{
	if (SubSettingData->SettingApplyType != ESAT_FunctionInCMD)
		return;

	SettingEntryObject->FunctionNameToApply = FunctionNameToApplyInCMD + " " + Value;
}

void USettingMenuListEntry::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim)
{
	if (!AnimToPlay)
		return;

	if (bPlayForwardAnim)
		PlayAnimationForward(AnimToPlay);
	else
		PlayAnimationReverse(AnimToPlay);
}

void USettingMenuListEntry::EnableEntry(bool bEnable)
{
	SubSettingData->bEntryWidgetEnabled = bEnable;

	SubSettingNameText->SetIsEnabled(bEnable);
	CategoryNameText->SetIsEnabled(bEnable);

	SubSettingQualityText->SetIsEnabled(bEnable);

	LeftArrowButton->SetIsEnabled(bEnable);
	RightArrowButton->SetIsEnabled(bEnable);

	if (SubSettingData->QualityCurrentValue == 0)
	{
		LeftArrowButton->SetIsEnabled(false);
	}
	else if (SubSettingData->QualityCurrentValue == SubSettingData->QualityTypes.Num() - 1)
	{
		RightArrowButton->SetIsEnabled(false);
	}

	SubSettingOnOffCheckBox->SetIsEnabled(bEnable);
	SubSettingOnOffButton->SetIsEnabled(bEnable);

	SliderButton->SetIsEnabled(bEnable);
	SubSettingSlider->SetIsEnabled(bEnable);
	SubSettingSliderValueText->SetIsEnabled(bEnable);

	KeyMappingButton->SetIsEnabled(bEnable);
	KeyMappingInputKeySelector->SetIsEnabled(bEnable);
}