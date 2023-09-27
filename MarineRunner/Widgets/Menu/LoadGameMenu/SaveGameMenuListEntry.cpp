// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/SaveGameMenuListEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "LoadGameMenuEntryObject.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

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

	SaveNameText->SetText(FText::FromString(ListEntryObject->SavesMenuData.SaveName));

	FText Date = FText::FromString("SAVED: " + ListEntryObject->SavesMenuData.SaveDateTime);
	SaveDateText->SetText(Date);

	ConvertTotalPlayTimeInSecondsToText();

	SetScreenshotImageFromSave();
}

void USaveGameMenuListEntry::OnClickedLoadGameButton()
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance) == false || IsValid(ListEntryObject) == false) 
		return;

	GameInstance->SlotSaveGameNameToLoad = ListEntryObject->SavesMenuData.SaveName;

	UGameplayStatics::OpenLevel(GetWorld(), FName(ListEntryObject->SavesMenuData.LevelNameToLoad));
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

void USaveGameMenuListEntry::ConvertTotalPlayTimeInSecondsToText()
{
	const int SecondsInHour = 3600;
	int32 Hours = ListEntryObject->SavesMenuData.TotalPlayTimeInSeconds / SecondsInHour;
	int32 RestMinutes = (ListEntryObject->SavesMenuData.TotalPlayTimeInSeconds - (Hours * SecondsInHour)) / 60.f;

	FString MinutesString = RestMinutes < 10 ? "0" + FString::FromInt(RestMinutes) : FString::FromInt(RestMinutes);

	FText TotalTime = FText::FromString("TOTAL TIME: " + FString::FromInt(Hours) + "H " + MinutesString + "MIN");

	TotalTimeText->SetText(TotalTime);
}

void USaveGameMenuListEntry::SetScreenshotImageFromSave()
{
	UTexture2D* ScreenshotFromSaveGame = UKismetRenderingLibrary::ImportFileAsTexture2D(GetWorld(), ListEntryObject->SavesMenuData.ScreenshotPathSave);
	if (IsValid(ScreenshotFromSaveGame) == false) 
		return;

	ScreenshotSaveImage->SetBrushFromTexture(ScreenshotFromSaveGame);
}
