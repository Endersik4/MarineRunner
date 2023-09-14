// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/SettingsMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"

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
}

void USettingsMenuWidget::FillMenuButtonsAndTextMap()
{
	MenuButtonsAndText.Add(GameSettingsButton, GameSettingsText);
	MenuButtonsAndText.Add(AudioSettingsButton, AudioSettingsText);
	MenuButtonsAndText.Add(VideoSettingsButton, VideoSettingsText);
	MenuButtonsAndText.Add(BindingsSettingsButton, BindingsSettingsText);
}

void USettingsMenuWidget::OnHoveredButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim, bool bCanHoverGivenText)
{
	if (bCanHoverGivenText || AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}

#pragma region //////////////// GAME SETTINGS /////////////////
void USettingsMenuWidget::OnClickedGameSettingsButton()
{
}

void USettingsMenuWidget::OnHoveredGameSettingsButton()
{
	OnHoveredButton(GameSettingsHoverAnim);
}

void USettingsMenuWidget::OnUnhoveredGameSettingsButton()
{
	OnHoveredButton(GameSettingsHoverAnim, false);
}
#pragma endregion

#pragma region //////////////// AUDIO SETTINGS /////////////////
void USettingsMenuWidget::OnClickedAudioSettingsButton()
{
}

void USettingsMenuWidget::OnHoveredAudioSettingsButton()
{
	OnHoveredButton(AudioSettingsHoverAnim);

}

void USettingsMenuWidget::OnUnhoveredAudioSettingsButton()
{
	OnHoveredButton(AudioSettingsHoverAnim, false);

}
#pragma endregion

#pragma region //////////////// VIDEO SETTINGS /////////////////
void USettingsMenuWidget::OnClickedVideoSettingsButton()
{
}

void USettingsMenuWidget::OnHoveredVideoSettingsButton()
{
	OnHoveredButton(VideoSettingsHoverAnim);

}

void USettingsMenuWidget::OnUnhoveredVideoSettingsButton()
{
	OnHoveredButton(VideoSettingsHoverAnim, false);

}
#pragma endregion

#pragma region //////////////// BINDINGS SETTINGS /////////////////
void USettingsMenuWidget::OnClickedBindingsSettingsButton()
{
}

void USettingsMenuWidget::OnHoveredBindingsSettingsButton()
{
	OnHoveredButton(BindingsSettingsHoverAnim);

}

void USettingsMenuWidget::OnUnhoveredBindingsSettingsButton()
{
	OnHoveredButton(BindingsSettingsHoverAnim, false);

}
#pragma endregion

#pragma region //////////////// ACCEPT SETTINGS /////////////////

void USettingsMenuWidget::OnClickedAcceptSettingsButton()
{

}

void USettingsMenuWidget::OnHoveredAcceptSettingsButton()
{
	OnHoveredButton(AcceptSettingsHoverAnim);

}

void USettingsMenuWidget::OnUnhoveredAcceptSettingsButton()
{
	OnHoveredButton(AcceptSettingsHoverAnim, false);

}
#pragma endregion

#pragma region //////////////// DEFAULTS SETTINGS /////////////////

void USettingsMenuWidget::OnClickedDefaultSettingsButton()
{

}

void USettingsMenuWidget::OnHoveredDefaultSettingsButton()
{
	OnHoveredButton(DefaultsSettingsHoverAnim);

}

void USettingsMenuWidget::OnUnhoveredDefaultSettingsButton()
{
	OnHoveredButton(DefaultsSettingsHoverAnim, false);
}
#pragma endregion