// Copyright Adam Bartela.All Rights Reserved


#include "SaveToLoadEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "MarineRunner/Player/SaveLoadGame/SaveMarineRunner.h"
#include "ConfirmLoadingGameWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "SaveToLoadEntryObject.h"

void USaveGameMenuListEntry::NativeConstruct()
{
	LoadGameButton->OnClicked.AddDynamic(this, &USaveGameMenuListEntry::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &USaveGameMenuListEntry::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &USaveGameMenuListEntry::OnUnhoveredLoadGameButton);
}

void USaveGameMenuListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (IsValid(ListItemObject))
		return;

	LoadGameEntryObject = Cast<ULoadGameMenuEntryObject>(ListItemObject);
	if (IsValid(LoadGameEntryObject))
		return;

	SaveNameText->SetText(FText::FromString("-" + LoadGameEntryObject->SavesMenuData.SaveName + "-"));

	const FText& Date = FText::FromString(SavedDateText + LoadGameEntryObject->SavesMenuData.SaveDateTime + "-");
	SaveDateText->SetText(Date);

	ConvertTotalPlayTimeInSecondsToText();

	SetScreenshotImageFromSave();
}

void USaveGameMenuListEntry::OnClickedLoadGameButton()
{
	if (!IsValid(LoadGameEntryObject))
		return;

	if (bShowConfirmLoadingWidget)
	{
		ShowConfirmLoadingWidget();
		return;
	}

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(GameInstance))
		return;
	GameInstance->SlotSaveGameNameToLoad = LoadGameEntryObject->SavesMenuData.SaveName;

	UGameplayStatics::OpenLevel(GetWorld(), FName(LoadGameEntryObject->SavesMenuData.LevelNameToLoad));

	return;
}

void USaveGameMenuListEntry::ShowConfirmLoadingWidget()
{
	if (!IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		return;

	TObjectPtr<UConfirmLoadingGameWidget> ConfirmLoadingWidget = Cast<UConfirmLoadingGameWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), ConfirmLoadingSaveWidgetClass));
	if (!IsValid(ConfirmLoadingWidget))
		return;

	ConfirmLoadingWidget->AddToViewport();
	ConfirmLoadingWidget->SetSlotAndLevelName(LoadGameEntryObject->SavesMenuData.SaveName, LoadGameEntryObject->SavesMenuData.LevelNameToLoad);
	ConfirmLoadingWidget->SetConfirmType(ECT_LoadLastSave);
}

void USaveGameMenuListEntry::OnHoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim);
}

void USaveGameMenuListEntry::OnUnhoveredLoadGameButton()
{
	PlayAnimatonForButton(LoadGameHoverAnim, false);
}

void USaveGameMenuListEntry::PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim)
{
	if (!AnimToPlay) 
		return;

	if (bPlayForwardAnim) 
		PlayAnimationForward(AnimToPlay);
	else 
		PlayAnimationReverse(AnimToPlay);
}

void USaveGameMenuListEntry::ConvertTotalPlayTimeInSecondsToText()
{
	const int SecondsInHour = 3600;
	const int32& Hours = LoadGameEntryObject->SavesMenuData.TotalPlayTimeInSeconds / SecondsInHour;
	const int32& RestMinutes = (LoadGameEntryObject->SavesMenuData.TotalPlayTimeInSeconds - (Hours * SecondsInHour)) / 60.f;

	const FString& MinutesString = RestMinutes < 10 ? "0" + FString::FromInt(RestMinutes) : FString::FromInt(RestMinutes);

	const FText& TotalTime = FText::FromString(SavedTotalTimeText + FString::FromInt(Hours) + "H " + MinutesString + "MIN" + "-");

	TotalTimeText->SetText(TotalTime);
}

void USaveGameMenuListEntry::SetScreenshotImageFromSave()
{
	TObjectPtr<UTexture2D> ScreenshotFromSaveGame = UKismetRenderingLibrary::ImportFileAsTexture2D(GetWorld(), LoadGameEntryObject->SavesMenuData.ScreenshotPathSave);
	if (!IsValid(ScreenshotFromSaveGame)) 
		return;

	ScreenshotSaveImage->SetBrushFromTexture(ScreenshotFromSaveGame);
}
