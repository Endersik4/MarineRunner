// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/SaveGameMenuListEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "LoadGameMenuEntryObject.h"

void USaveGameMenuListEntry::NativeConstruct()
{
	LoadGameButton->OnClicked.AddDynamic(this, &USaveGameMenuListEntry::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &USaveGameMenuListEntry::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &USaveGameMenuListEntry::OnUnhoveredLoadGameButton);
}

void USaveGameMenuListEntry::NativeOnInitialized()
{
}

void USaveGameMenuListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ListEntryObject = Cast<ULoadGameMenuEntryObject>(ListItemObject);
}

void USaveGameMenuListEntry::OnClickedLoadGameButton()
{
}

void USaveGameMenuListEntry::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim);
}

void USaveGameMenuListEntry::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false);
}

void USaveGameMenuListEntry::PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim)
{
	if (AnimToPlay == nullptr) return;

	if (bPlayForwardAnim) PlayAnimationForward(AnimToPlay);
	else PlayAnimationReverse(AnimToPlay);
}
