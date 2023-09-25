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

	int32 Hours = ListEntryObject->SavesMenuData.TotalPlayTimeInSeconds / 3600;
	int32 RestMinutes = (ListEntryObject->SavesMenuData.TotalPlayTimeInSeconds - (Hours * 3600)) / 60.f;
	FText TotalTime = FText::FromString("TOTAL TIME: " + FString::FromInt(Hours) + "H "+ FString::FromInt(RestMinutes) + "MIN");
	TotalTimeText->SetText(TotalTime);

	UTexture2D* ScreenshotFromSaveGame = UKismetRenderingLibrary::ImportFileAsTexture2D(GetWorld(), ListEntryObject->SavesMenuData.ScreenshotPathSave);
	if (ScreenshotFromSaveGame)
		ScreenshotSaveImage->SetBrushFromTexture(ScreenshotFromSaveGame);


	/*HighResShot 1920x1080 filename="D:/Projekty z UE4/MarineRunner/Saved/SaveGames/plik.png"
	FString SaveGamesDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/plik.png";
	FString TakeScreenShotCommand = "HighResShot 640x360 filename=\"";
	TakeScreenShotCommand += SaveGamesDir + "\"";
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand(TakeScreenShotCommand);
	FString SaveNameFile = GetScreenshotSaveDir();
	SaveNameFile += "ScreenShot00000.png";*/
}

void USaveGameMenuListEntry::OnClickedLoadGameButton()
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance) == false || IsValid(ListEntryObject) == false) 
		return;

	GameInstance->SlotSaveGameNameToLoad = ListEntryObject->SavesMenuData.SaveName;

	UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
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
