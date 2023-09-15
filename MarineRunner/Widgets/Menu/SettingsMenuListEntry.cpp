// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/SettingsMenuListEntry.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Kismet/KismetTextLibrary.h"


void USettingsMenuListEntry::NativeConstruct()
{
	LeftArrowButton->OnClicked.AddDynamic(this, &USettingsMenuListEntry::OnClickedLeftArrowButton);
	LeftArrowButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredLeftArrowButton);
	LeftArrowButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredLeftArrowButton);

	RightArrowButton->OnClicked.AddDynamic(this, &USettingsMenuListEntry::OnClickedRightArrowButton);
	RightArrowButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredRightArrowButton);
	RightArrowButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredRightArrowButton);

	CurrentSettingsCheckBoxButton->OnClicked.AddDynamic(this, &USettingsMenuListEntry::OnClickedCheckBoxButton);
	CurrentSettingsCheckBoxButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredCheckBoxButton);
	CurrentSettingsCheckBoxButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredCheckBoxButton);

	CurrentSettingsSlider->OnValueChanged.AddDynamic(this, &USettingsMenuListEntry::OnValueChangedSlider);
	SliderButton->OnHovered.AddDynamic(this, &USettingsMenuListEntry::OnHoveredSliderButton);
	SliderButton->OnUnhovered.AddDynamic(this, &USettingsMenuListEntry::OnUnhoveredSliderButton);
}

void USettingsMenuListEntry::NativeOnInitialized()
{
}

void USettingsMenuListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ListEntryObject = Cast<USettingsMenuEntryObject>(ListItemObject);
	HideAllUIElements();
	MenuSettingsData = ListEntryObject->MenuSettingsData;

	if (MenuSettingsData.SettingsType == EST_Category)
	{
		SettingMenu_Category();
		return;
	}

	SettingsText->SetText(MenuSettingsData.SettingsName);
	SettingsText->SetVisibility(ESlateVisibility::Visible);
	SettingsText->SetIsEnabled(true);

	if (MenuSettingsData.SettingsType == EST_List)
	{
		SettingMenu_List();
		return;
	}

	if (MenuSettingsData.SettingsType == EST_KeyBindings)
	{
		SettingMenu_KeyBindings();
		return;
	}

	if (MenuSettingsData.SettingsType == EST_Slider)
	{
		SettingMenu_Slider();
		return;
	}

	if (MenuSettingsData.SettingsType == EST_Checked)
	{
		SettingMenu_CheckBox();
		return;
	}
}

#pragma region ///////// LEFT ARROW ////////////
void USettingsMenuListEntry::OnClickedLeftArrowButton()
{
	MenuSettingsData.CurrentSettingsListValue--;
	CurrentSettingsChoice->SetText(MenuSettingsData.SettingsApplyTextes[MenuSettingsData.CurrentSettingsListValue]);

	if (MenuSettingsData.CurrentSettingsListValue == 0) LeftArrowButton->SetIsEnabled(false);
	RightArrowButton->SetIsEnabled(true);
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
	MenuSettingsData.CurrentSettingsListValue++;
	CurrentSettingsChoice->SetText(MenuSettingsData.SettingsApplyTextes[MenuSettingsData.CurrentSettingsListValue]);

	if (MenuSettingsData.CurrentSettingsListValue == MenuSettingsData.SettingsApplyTextes.Num() - 1) RightArrowButton->SetIsEnabled(false);
	LeftArrowButton->SetIsEnabled(true);
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
void USettingsMenuListEntry::OnClickedCheckBoxButton()
{
	CurrentSettingsCheckBox->SetCheckedState(bIsChecked ? ECheckBoxState::Unchecked : ECheckBoxState::Checked);
	bIsChecked = bIsChecked ? false : true;
}

void USettingsMenuListEntry::OnHoveredCheckBoxButton()
{
	OnHoveredButton(CheckBoxHoverAnim);
}

void USettingsMenuListEntry::OnUnhoveredCheckBoxButton()
{
	OnHoveredButton(CheckBoxHoverAnim, false);
}
#pragma endregion

#pragma region ///////// SLIDER ////////////
void USettingsMenuListEntry::OnValueChangedSlider(float Value)
{
	FString NewNumber = "-";
	NewNumber += UKismetTextLibrary::Conv_FloatToText(Value, ERoundingMode::HalfToEven, false, true, 1, 3, 1, 2).ToString();
	NewNumber += "-";
	SliderNumber->SetText(FText::FromString(NewNumber));

	MenuSettingsData.CurrentSettingsValue = Value;
	if (ListEntryObject) ListEntryObject->MenuSettingsData = MenuSettingsData;
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

void USettingsMenuListEntry::HideAllUIElements()
{
	HorizontalBoxForSettingsText->SetVisibility(ESlateVisibility::Hidden);
	CategoryText->SetVisibility(ESlateVisibility::Hidden);

	SettingsText->SetVisibility(ESlateVisibility::Hidden);
	CurrentSettingsChoice->SetVisibility(ESlateVisibility::Hidden);

	LeftArrowButton->SetVisibility(ESlateVisibility::Hidden);
	RightArrowButton->SetVisibility(ESlateVisibility::Hidden);

	CurrentSettingsCheckBox->SetVisibility(ESlateVisibility::Hidden);
	CurrentSettingsCheckBoxButton->SetVisibility(ESlateVisibility::Hidden);

	CurrentSettingsSlider->SetVisibility(ESlateVisibility::Hidden);
	SliderButton->SetVisibility(ESlateVisibility::Hidden);
	SliderNumber->SetVisibility(ESlateVisibility::Hidden);
}

void USettingsMenuListEntry::OnHoveredButton(UWidgetAnimation* AnimToPlay,bool bPlayForwardAnim)
{
	if (AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}

#pragma region ////////////// SET SETTING TYPE ////////////////
void USettingsMenuListEntry::SettingMenu_Category()
{
	CategoryText->SetText(MenuSettingsData.SettingsName);
	CategoryText->SetColorAndOpacity(CategoryTextColor);

	CategoryText->SetVisibility(ESlateVisibility::Visible);
	HorizontalBoxForSettingsText->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuListEntry::SettingMenu_List()
{
	CurrentSettingsChoice->SetVisibility(ESlateVisibility::Visible);

	LeftArrowButton->SetVisibility(ESlateVisibility::Visible);
	RightArrowButton->SetVisibility(ESlateVisibility::Visible);
	CurrentSettingsChoice->SetText(MenuSettingsData.SettingsApplyTextes[MenuSettingsData.CurrentSettingsListValue]);
	if (MenuSettingsData.CurrentSettingsListValue == 0) LeftArrowButton->SetIsEnabled(false);
	if (MenuSettingsData.CurrentSettingsListValue == MenuSettingsData.SettingsApplyTextes.Num() - 1) RightArrowButton->SetIsEnabled(false);
}

void USettingsMenuListEntry::SettingMenu_KeyBindings()
{
	CurrentSettingsChoice->SetVisibility(ESlateVisibility::Visible);
	CurrentSettingsChoice->SetText(MenuSettingsData.SettingsKeyBindingsValue);
}

void USettingsMenuListEntry::SettingMenu_Slider()
{
	CurrentSettingsSlider->SetVisibility(ESlateVisibility::Visible);
	SliderButton->SetVisibility(ESlateVisibility::Visible);
	SliderNumber->SetVisibility(ESlateVisibility::Visible);

	CurrentSettingsSlider->SetMaxValue(MenuSettingsData.RangeOfSlider.GetUpperBoundValue());
	CurrentSettingsSlider->SetMinValue(MenuSettingsData.RangeOfSlider.GetLowerBoundValue());
	
}

void USettingsMenuListEntry::SettingMenu_CheckBox()
{
	CurrentSettingsCheckBox->SetVisibility(ESlateVisibility::Visible);
	CurrentSettingsCheckBoxButton->SetVisibility(ESlateVisibility::Visible);
	bIsChecked = MenuSettingsData.bChecked;

	if (bIsChecked) CurrentSettingsCheckBox->SetCheckedState(ECheckBoxState::Checked);
	else CurrentSettingsCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
}
#pragma endregion