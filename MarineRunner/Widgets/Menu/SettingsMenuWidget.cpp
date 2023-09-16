// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/SettingsMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Widgets/Menu/SettingsMenuEntryObject.h"
#include "MarineRunner/Widgets/Menu/SettingsMenuListEntry.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	GameSettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnClickedGameSettingsButton);
	GameSettingsButton->OnHovered.AddDynamic(this, &USettingsMenuWidget::OnHoveredGameSettingsButton);
	GameSettingsButton->OnUnhovered.AddDynamic(this, &USettingsMenuWidget::OnUnhoveredGameSettingsButton);

	AudioSettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnClickedAudioSettingsButton);
	AudioSettingsButton->OnHovered.AddDynamic(this, &USettingsMenuWidget::OnHoveredAudioSettingsButton);
	AudioSettingsButton->OnUnhovered.AddDynamic(this, &USettingsMenuWidget::OnUnhoveredAudioSettingsButton);

	VideoSettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnClickedVideoSettingsButton);
	VideoSettingsButton->OnHovered.AddDynamic(this, &USettingsMenuWidget::OnHoveredVideoSettingsButton);
	VideoSettingsButton->OnUnhovered.AddDynamic(this, &USettingsMenuWidget::OnUnhoveredVideoSettingsButton);

	BindingsSettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnClickedBindingsSettingsButton);
	BindingsSettingsButton->OnHovered.AddDynamic(this, &USettingsMenuWidget::OnHoveredBindingsSettingsButton);
	BindingsSettingsButton->OnUnhovered.AddDynamic(this, &USettingsMenuWidget::OnUnhoveredBindingsSettingsButton);

	AcceptSettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnClickedAcceptSettingsButton);
	AcceptSettingsButton->OnHovered.AddDynamic(this, &USettingsMenuWidget::OnHoveredAcceptSettingsButton);
	AcceptSettingsButton->OnUnhovered.AddDynamic(this, &USettingsMenuWidget::OnUnhoveredAcceptSettingsButton);

	DefaultsSettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnClickedDefaultSettingsButton);
	DefaultsSettingsButton->OnHovered.AddDynamic(this, &USettingsMenuWidget::OnHoveredDefaultSettingsButton);
	DefaultsSettingsButton->OnUnhovered.AddDynamic(this, &USettingsMenuWidget::OnUnhoveredDefaultSettingsButton);
}

void USettingsMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FillMenuButtonsAndTextMap();

	PlayAnimation(GameSettingsHoverAnim, 0.17f);
	CurrentSettingChoice = FSettingChoiceStruct(GameSettingsButton, GameSettingsHoverAnim);
	FillCurrentMenuSettingsListView(GameSettingsList);

	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void USettingsMenuWidget::FillMenuButtonsAndTextMap()
{
	MenuButtonsAndText.Add(GameSettingsButton, GameSettingsText);
	MenuButtonsAndText.Add(AudioSettingsButton, AudioSettingsText);
	MenuButtonsAndText.Add(VideoSettingsButton, VideoSettingsText);
	MenuButtonsAndText.Add(BindingsSettingsButton, BindingsSettingsText);
}

void USettingsMenuWidget::FillCurrentMenuSettingsListView(const TArray<FMenuSettings>& DataToFillFrom)
{
	SettingsListView->ClearListItems();

	for (const FMenuSettings & CurrentSetting : DataToFillFrom)
	{
		USettingsMenuEntryObject* ConstructedItemObject = NewObject<USettingsMenuEntryObject>(MenuSettingsDataObject);
		ConstructedItemObject->MenuSettingsData = CurrentSetting;
		ConstructedItemObject->SettingMenuWidget = this;

		SettingsListView->AddItem(ConstructedItemObject);

		ConstructedItemObject->ConditionalBeginDestroy();
	}
}

void USettingsMenuWidget::OnHoveredButton(UWidgetAnimation* AnimToPlay, UButton* ButtonToHover, bool bPlayForwardAnim)
{
	if (ButtonToHover == CurrentSettingChoice.CurrentButton || AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}

#pragma region //////////////// GAME SETTINGS /////////////////
void USettingsMenuWidget::OnClickedGameSettingsButton()
{
	if (GameSettingsButton == CurrentSettingChoice.CurrentButton) return;

	OnHoveredButton(CurrentSettingChoice.AnimationToPlay, nullptr, false);
	CurrentSettingChoice = FSettingChoiceStruct(GameSettingsButton, GameSettingsHoverAnim);

	FillCurrentMenuSettingsListView(GameSettingsList);
}

void USettingsMenuWidget::OnHoveredGameSettingsButton()
{
	OnHoveredButton(GameSettingsHoverAnim, GameSettingsButton);
}

void USettingsMenuWidget::OnUnhoveredGameSettingsButton()
{
	OnHoveredButton(GameSettingsHoverAnim, GameSettingsButton, false);
}
#pragma endregion

#pragma region //////////////// AUDIO SETTINGS /////////////////
void USettingsMenuWidget::OnClickedAudioSettingsButton()
{
	if (AudioSettingsButton == CurrentSettingChoice.CurrentButton) return;

	OnHoveredButton(CurrentSettingChoice.AnimationToPlay, nullptr, false);
	CurrentSettingChoice = FSettingChoiceStruct(AudioSettingsButton, AudioSettingsHoverAnim);

	FillCurrentMenuSettingsListView(AudioSettingsList);
}

void USettingsMenuWidget::OnHoveredAudioSettingsButton()
{
	OnHoveredButton(AudioSettingsHoverAnim, AudioSettingsButton);
}

void USettingsMenuWidget::OnUnhoveredAudioSettingsButton()
{
	OnHoveredButton(AudioSettingsHoverAnim, AudioSettingsButton, false);

}
#pragma endregion

#pragma region //////////////// VIDEO SETTINGS /////////////////
void USettingsMenuWidget::OnClickedVideoSettingsButton()
{
	if (VideoSettingsButton == CurrentSettingChoice.CurrentButton) return;

	OnHoveredButton(CurrentSettingChoice.AnimationToPlay, nullptr, false);
	CurrentSettingChoice = FSettingChoiceStruct(VideoSettingsButton, VideoSettingsHoverAnim);

	FillCurrentMenuSettingsListView(VideoSettingsList);
}

void USettingsMenuWidget::OnHoveredVideoSettingsButton()
{
	OnHoveredButton(VideoSettingsHoverAnim, VideoSettingsButton);
}

void USettingsMenuWidget::OnUnhoveredVideoSettingsButton()
{
	OnHoveredButton(VideoSettingsHoverAnim, VideoSettingsButton, false);

}
#pragma endregion

#pragma region //////////////// BINDINGS SETTINGS /////////////////
void USettingsMenuWidget::OnClickedBindingsSettingsButton()
{
	if (BindingsSettingsButton == CurrentSettingChoice.CurrentButton) return;

	OnHoveredButton(CurrentSettingChoice.AnimationToPlay, nullptr, false);
	CurrentSettingChoice = FSettingChoiceStruct(BindingsSettingsButton, BindingsSettingsHoverAnim);

	FillCurrentMenuSettingsListView(BindingsSettingsList);
}

void USettingsMenuWidget::OnHoveredBindingsSettingsButton()
{
	OnHoveredButton(BindingsSettingsHoverAnim, BindingsSettingsButton);

}

void USettingsMenuWidget::OnUnhoveredBindingsSettingsButton()
{
	OnHoveredButton(BindingsSettingsHoverAnim, BindingsSettingsButton, false);

}
#pragma endregion

#pragma region //////////////// ACCEPT SETTINGS /////////////////

void USettingsMenuWidget::OnClickedAcceptSettingsButton()
{
	TArray<UObject*> AllListElements = SettingsListView->GetListItems();

	for (UObject* CurrentElement : AllListElements)
	{
		USettingsMenuEntryObject* SettingMenuObject = Cast<USettingsMenuEntryObject>(CurrentElement);
		if (SettingMenuObject == nullptr) continue;
		const FMenuSettings& SubSettingData = SettingMenuObject->MenuSettingsData;

		if (SubSettingData.SubSettingType == EST_Category) continue;

		if (SubSettingData.SettingApplyType == ESAT_FunctionInCMD)
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand(SubSettingData.SubSettingFunctionName);
			continue;
		}

		if (SubSettingData.SettingApplyType == ESAT_MouseSens)
		{
			if (MarinePawn == nullptr) continue;
			MarinePawn->ChangeMouseSensivity(SubSettingData.SliderCurrentValue);
			continue;
		}

		if (SubSettingData.SettingApplyType == ESAT_Sounds)
		{
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SubSettingData.SoundMixClassToChangeVolume, SubSettingData.SoundClassToChangeVolume, SubSettingData.SliderCurrentValue, 1.f, 0.f);
			UGameplayStatics::PushSoundMixModifier(GetWorld(), SubSettingData.SoundMixClassToChangeVolume);
			
			continue;
		}
	}
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("r.MotionBlur.Amount 1");
}

void USettingsMenuWidget::OnHoveredAcceptSettingsButton()
{
	OnHoveredButton(AcceptSettingsHoverAnim, nullptr);

}

void USettingsMenuWidget::OnUnhoveredAcceptSettingsButton()
{
	OnHoveredButton(AcceptSettingsHoverAnim, nullptr, false);

}
#pragma endregion

#pragma region //////////////// DEFAULTS SETTINGS /////////////////

void USettingsMenuWidget::OnClickedDefaultSettingsButton()
{

}

void USettingsMenuWidget::OnHoveredDefaultSettingsButton()
{
	OnHoveredButton(DefaultsSettingsHoverAnim, nullptr);

}

void USettingsMenuWidget::OnUnhoveredDefaultSettingsButton()
{
	OnHoveredButton(DefaultsSettingsHoverAnim, nullptr, false);
}
#pragma endregion
