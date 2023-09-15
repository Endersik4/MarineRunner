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
		class UTextBlock* SettingsText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CategoryText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CurrentSettingsChoice;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* LeftArrowButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* RightArrowButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UCheckBox* CurrentSettingsCheckBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* CurrentSettingsCheckBoxButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* SliderButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class USlider* CurrentSettingsSlider;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* SliderNumber;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LeftArrowHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* RightArrowHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CheckBoxHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SliderHoverAnim = nullptr;

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
		void OnClickedCheckBoxButton();
	UFUNCTION()
		void OnHoveredCheckBoxButton();
	UFUNCTION()
		void OnUnhoveredCheckBoxButton();

	UFUNCTION()
		void OnValueChangedSlider(float Value);
	UFUNCTION()
		void OnHoveredSliderButton();
	UFUNCTION()
		void OnUnhoveredSliderButton();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Text Settings")
		FLinearColor CategoryTextColor;

	void OnHoveredButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true);

	void HideAllUIElements();

	bool bIsChecked;

	FMenuSettings MenuSettingsData;

	// List 
	// 0 - Low, 1 - Medium, 2 - Hard, 3 - Ultra
	int32 Settings_ListValue = 3;

	// set this widget to be one of ESettingsType
	void SettingMenu_Category();
	void SettingMenu_List();
	void SettingMenu_KeyBindings();
	void SettingMenu_Slider();
	void SettingMenu_CheckBox();

	class USettingsMenuEntryObject* ListEntryObject;
};
