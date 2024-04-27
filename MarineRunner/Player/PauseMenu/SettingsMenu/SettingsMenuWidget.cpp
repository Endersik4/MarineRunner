// Copyright Adam Bartela.All Rights Reserved


#include "SettingsMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"

#include "SettingsMenuEntryObject.h"
#include "SettingsMenuListEntry.h"
#include "MarineRunner/Player/MarinePlayerController.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/SaveLoadGame/SaveLoadPlayerComponent.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

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

	if (ensureMsgf(UGameplayStatics::GetPlayerController(GetWorld(), 0), TEXT("Player Controller is nullptr in PauseMenuComponent!")))
	{
		MarinePlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
	if (ensureMsgf(UGameplayStatics::GetGameInstance(GetWorld()), TEXT("Game Instance is nullptr in SettingsMenuWidget!")))
	{
		MarineGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	FillMenuButtonsAndTextMap();

	StartOnGameSettingsList();
}

void USettingsMenuWidget::FillMenuButtonsAndTextMap()
{
	MenuButtonsAndText.Add(GameSettingsButton, GameSettingsText);
	MenuButtonsAndText.Add(AudioSettingsButton, AudioSettingsText);
	MenuButtonsAndText.Add(VideoSettingsButton, VideoSettingsText);
	MenuButtonsAndText.Add(BindingsSettingsButton, BindingsSettingsText);
}

void USettingsMenuWidget::StartOnGameSettingsList()
{
	PlayAnimation(GameSettingsHoverAnim, GameSettingsHoverAnim->GetEndTime());
	CurrentSelectedSetting = FSettingSelected(GameSettingsButton, GameSettingsHoverAnim);
	FillCurrentMenuSettingsListView(GameSettingsList);
}

void USettingsMenuWidget::FillCurrentMenuSettingsListView(const TArray<FMenuSettings>& DataToFillFrom)
{
	SettingsListView->ClearListItems();

	USettingsMenuEntryObject* ObjectThatConnectOtherSettings = nullptr;
	for (const FMenuSettings& CurrentSetting : DataToFillFrom)
	{
		TObjectPtr<USettingsMenuEntryObject> ConstructedItemObject = NewObject<USettingsMenuEntryObject>(MenuSettingsDataObject);
		if (!IsValid(ConstructedItemObject)) 
			continue;

		ConstructedItemObject->MenuSettingsData = CurrentSetting;
		ConstructedItemObject->SetGameInstance(MarineGameInstance);
		ConstructedItemObject->SetVariablesToCurrent();

		ObjectThatConnectOtherSettings = ConnectedSettings(ConstructedItemObject, ObjectThatConnectOtherSettings);
		
		SettingsListView->AddItem(ConstructedItemObject);
	}
}

TObjectPtr<USettingsMenuEntryObject> USettingsMenuWidget::ConnectedSettings(TObjectPtr<USettingsMenuEntryObject> ConstructedItemObject, TObjectPtr<USettingsMenuEntryObject> ObjectThatConnectOtherSettings)
{
	if (ConstructedItemObject->MenuSettingsData.bIsItObjectThatConnects)
	{
		ConstructedItemObject->SettingMenuWidget = this;
		ObjectThatConnectOtherSettings = ConstructedItemObject;

		return ObjectThatConnectOtherSettings;
	}

	if (!IsValid(ObjectThatConnectOtherSettings))
		return ObjectThatConnectOtherSettings;

	if (ObjectThatConnectOtherSettings->MenuSettingsData.bIsItObjectThatConnects && ConstructedItemObject->MenuSettingsData.bIsConnectedToOtherSettings)
	{
		ConstructedItemObject->MenuSettingsData.bEntryWidgetEnabled = ObjectThatConnectOtherSettings->MenuSettingsData.bSettingEnabled;
	}
	else if (!ConstructedItemObject->MenuSettingsData.bIsConnectedToOtherSettings)
	{
		ObjectThatConnectOtherSettings = nullptr;
	}

	return ObjectThatConnectOtherSettings;
}

void USettingsMenuWidget::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, TObjectPtr<UButton> HoveredButton, bool bPlayForwardAnim)
{
	if (HoveredButton == CurrentSelectedSetting.SelectedButton || !AnimToPlay) 
		return;

	if (bPlayForwardAnim) 
		PlayAnimationForward(AnimToPlay);
	else 
		PlayAnimationReverse(AnimToPlay);
}

#pragma region //////////////// GAME SETTINGS /////////////////
void USettingsMenuWidget::OnClickedGameSettingsButton()
{
	if (GameSettingsButton == CurrentSelectedSetting.SelectedButton) 
		return;

	PlayAnimatonForButton(CurrentSelectedSetting.AnimationToPlay, nullptr, false);
	CurrentSelectedSetting = FSettingSelected(GameSettingsButton, GameSettingsHoverAnim);

	FillCurrentMenuSettingsListView(GameSettingsList);
}

void USettingsMenuWidget::OnHoveredGameSettingsButton()
{
	PlayAnimatonForButton(GameSettingsHoverAnim, GameSettingsButton);
}

void USettingsMenuWidget::OnUnhoveredGameSettingsButton()
{
	PlayAnimatonForButton(GameSettingsHoverAnim, GameSettingsButton, false);
}
#pragma endregion

#pragma region //////////////// AUDIO SETTINGS /////////////////
void USettingsMenuWidget::OnClickedAudioSettingsButton()
{
	if (AudioSettingsButton == CurrentSelectedSetting.SelectedButton) 
		return;

	PlayAnimatonForButton(CurrentSelectedSetting.AnimationToPlay, nullptr, false);
	CurrentSelectedSetting = FSettingSelected(AudioSettingsButton, AudioSettingsHoverAnim);

	FillCurrentMenuSettingsListView(AudioSettingsList);
}

void USettingsMenuWidget::OnHoveredAudioSettingsButton()
{
	PlayAnimatonForButton(AudioSettingsHoverAnim, AudioSettingsButton);
}

void USettingsMenuWidget::OnUnhoveredAudioSettingsButton()
{
	PlayAnimatonForButton(AudioSettingsHoverAnim, AudioSettingsButton, false);

}
#pragma endregion

#pragma region //////////////// VIDEO SETTINGS /////////////////
void USettingsMenuWidget::OnClickedVideoSettingsButton()
{
	if (VideoSettingsButton == CurrentSelectedSetting.SelectedButton) 
		return;

	PlayAnimatonForButton(CurrentSelectedSetting.AnimationToPlay, nullptr, false);
	CurrentSelectedSetting = FSettingSelected(VideoSettingsButton, VideoSettingsHoverAnim);

	FillCurrentMenuSettingsListView(VideoSettingsList);
}

void USettingsMenuWidget::OnHoveredVideoSettingsButton()
{
	PlayAnimatonForButton(VideoSettingsHoverAnim, VideoSettingsButton);
}

void USettingsMenuWidget::OnUnhoveredVideoSettingsButton()
{
	PlayAnimatonForButton(VideoSettingsHoverAnim, VideoSettingsButton, false);

}
#pragma endregion

#pragma region //////////////// BINDINGS SETTINGS /////////////////
void USettingsMenuWidget::OnClickedBindingsSettingsButton()
{
	if (BindingsSettingsButton == CurrentSelectedSetting.SelectedButton) 
		return;

	PlayAnimatonForButton(CurrentSelectedSetting.AnimationToPlay, nullptr, false);
	CurrentSelectedSetting = FSettingSelected(BindingsSettingsButton, BindingsSettingsHoverAnim);

	FillCurrentMenuSettingsListView(BindingsSettingsList);
}

void USettingsMenuWidget::OnHoveredBindingsSettingsButton()
{
	PlayAnimatonForButton(BindingsSettingsHoverAnim, BindingsSettingsButton);

}

void USettingsMenuWidget::OnUnhoveredBindingsSettingsButton()
{
	PlayAnimatonForButton(BindingsSettingsHoverAnim, BindingsSettingsButton, false);

}
#pragma endregion

#pragma region //////////////// ACCEPT SETTINGS /////////////////

void USettingsMenuWidget::OnClickedAcceptSettingsButton()
{
	SpawnSettingsAcceptedWidget();

	for (UObject* CurrentElement : SettingsListView->GetListItems())
	{
		const TObjectPtr<USettingsMenuEntryObject> SettingMenuObject = Cast<USettingsMenuEntryObject>(CurrentElement);
		if (!IsValid(SettingMenuObject)) 
			continue;

		ActiveSettingByType(SettingMenuObject->MenuSettingsData, SettingMenuObject);

		ReplaceValueInGameInstanceByName(SettingMenuObject->MenuSettingsData);
	}

	UGameUserSettings::GetGameUserSettings()->ApplyResolutionSettings(false);
	MarineGameInstance->SaveCustomSavedSettingsToConfig();

	LoadSavedSettingsToPlayer();
}

void USettingsMenuWidget::ActiveSettingByType(const FMenuSettings& SubSettingData, TObjectPtr<USettingsMenuEntryObject> SettingMenuObject)
{
	if (!SubSettingData.bEntryWidgetEnabled || SubSettingData.SubSettingType == EST_Category)
		return;

	if (SubSettingData.SubSettingType == EST_SetFullscreen)
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(SubSettingData.AllWindowTypes[SubSettingData.QualityCurrentValue]);
	}
	else if (SubSettingData.SubSettingType == EST_SetResolution)
	{
		UGameUserSettings::GetGameUserSettings()->SetScreenResolution(SubSettingData.SupportedResolutionsList[SubSettingData.QualityCurrentValue]);
	}

	if (SubSettingData.SettingApplyType == ESAT_FunctionInCMD && IsValid(MarinePlayerController))
	{
		MarinePlayerController->ConsoleCommand(SettingMenuObject->FunctionNameToApply);
		return;
	}

	if (SubSettingData.SettingApplyType == ESAT_Sounds)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SubSettingData.SoundMixClassToChangeVolume, SubSettingData.SoundClassToChangeVolume, SubSettingData.SliderCurrentValue, 1.f, 0.f);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), SubSettingData.SoundMixClassToChangeVolume);

		return;
	}
}

void USettingsMenuWidget::ReplaceValueInGameInstanceByName(const FMenuSettings& SubSettingData)
{
	if (!SubSettingData.bSaveValueToGameInstance)
		return;

	if (SubSettingData.SubSettingType == EST_Quality)
	{
		MarineGameInstance->ReplaceValueInSavedSettingByName(SubSettingData.QualityCurrentValue, SubSettingData.SavedValueName);
	}
	else if (SubSettingData.SubSettingType == EST_SliderValue)
	{
		MarineGameInstance->ReplaceValueInSavedSettingByName(SubSettingData.SliderCurrentValue, SubSettingData.SavedValueName);
	}
}

void USettingsMenuWidget::SpawnSettingsAcceptedWidget()
{
	if (!IsValid(SettingsAppliedWidgetClass))
		return;

	TObjectPtr<UUserWidget> SpawnedWidget = CreateWidget(MarinePlayerController, SettingsAppliedWidgetClass);
	if (!IsValid(SpawnedWidget))
		return;

	SpawnedWidget->AddToViewport();
}

void USettingsMenuWidget::LoadSavedSettingsToPlayer()
{
	if (!IsValid(MarinePlayerController))
		return;

	TObjectPtr<AMarineCharacter> Player = Cast<AMarineCharacter>(MarinePlayerController->GetPawn());
	if (!IsValid(Player))
		return;

	Player->GetWeaponHandlerComponent()->LoadSavedMouseSensitivities();
	Player->LoadFieldOfViewFromSettings();
}

void USettingsMenuWidget::OnHoveredAcceptSettingsButton()
{
	PlayAnimatonForButton(AcceptSettingsHoverAnim, nullptr);

}

void USettingsMenuWidget::OnUnhoveredAcceptSettingsButton()
{
	PlayAnimatonForButton(AcceptSettingsHoverAnim, nullptr, false);

}
#pragma endregion

#pragma region //////////////// DEFAULTS SETTINGS /////////////////

void USettingsMenuWidget::OnClickedDefaultSettingsButton()
{

}

void USettingsMenuWidget::OnHoveredDefaultSettingsButton()
{
	PlayAnimatonForButton(DefaultsSettingsHoverAnim, nullptr);

}

void USettingsMenuWidget::OnUnhoveredDefaultSettingsButton()
{
	PlayAnimatonForButton(DefaultsSettingsHoverAnim, nullptr, false);
}
#pragma endregion
