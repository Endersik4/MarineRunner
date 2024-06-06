// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/GameMenu/LoadGameMenu/LoadSaveEntry.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "MarineRunner/Player/SaveLoadGame/SaveMarineRunner.h"
#include "MarineRunner/Player/GameMenu/ConfirmOptionWidget.h"
#include "MarineRunner/Player/GameMenu/GameMenuBase.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "LoadSaveEntryObject.h"

void ULoadSaveEntry::NativeConstruct()
{
	LoadGameButton->OnClicked.AddDynamic(this, &ULoadSaveEntry::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &ULoadSaveEntry::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &ULoadSaveEntry::OnUnhoveredLoadGameButton);
}

void ULoadSaveEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!IsValid(ListItemObject))
		return;

	LoadGameEntryObject = Cast<ULoadSaveEntryObject>(ListItemObject);
	if (!IsValid(LoadGameEntryObject))
		return;

	SaveNameText->SetText(FText::FromString("-" + LoadGameEntryObject->SavesMenuData.SaveName + "-"));

	const FText& Date = FText::FromString(SavedDateText + LoadGameEntryObject->SavesMenuData.SaveDateTime + "-");
	SaveDateText->SetText(Date);

	ConvertTotalPlayTimeInSecondsToText();

	SetScreenshotImageFromSave();
}

void ULoadSaveEntry::OnClickedLoadGameButton()
{
	if (!IsValid(LoadGameEntryObject))
		return;

	if (bShowConfirmLoadingWidget)
	{
		ShowConfirmLoadingWidget();
		return;
	}

	LoadSave();

	return;
}

void ULoadSaveEntry::ShowConfirmLoadingWidget()
{
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PlayerController))
		return;

	TObjectPtr<UConfirmOptionWidget> ConfirmLoadingWidget = Cast<UConfirmOptionWidget>(CreateWidget(PlayerController, ConfirmOptionWidgetClass));
	if (!IsValid(ConfirmLoadingWidget))
		return;

	ConfirmLoadingWidget->AddToViewport();
	ConfirmLoadingWidget->SetCurrentSpawnedMenu(LoadGameEntryObject->CurrentSpawnedMenu);
	ConfirmLoadingWidget->AddThisWidgetToCurrentSpawnedMenuWidgets(false);
	ConfirmLoadingWidget->ConfirmFunction = [this]() {this->LoadSave(); };
}

void ULoadSaveEntry::LoadSave()
{
	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(GameInstance))
		return;
	GameInstance->SlotSaveGameNameToLoad = LoadGameEntryObject->SavesMenuData.SaveName;

	UGameplayStatics::OpenLevel(GetWorld(), FName(LoadGameEntryObject->SavesMenuData.LevelNameToLoad));
}

void ULoadSaveEntry::OnHoveredLoadGameButton()
{
	PlayAnimationForward(LoadGameHoverAnim);
}

void ULoadSaveEntry::OnUnhoveredLoadGameButton()
{
	PlayAnimationReverse(LoadGameHoverAnim);
}

void ULoadSaveEntry::ConvertTotalPlayTimeInSecondsToText()
{
	const int SecondsInHour = 3600;
	const int32& Hours = LoadGameEntryObject->SavesMenuData.TotalPlayTimeInSeconds / SecondsInHour;
	const int32& RestMinutes = (LoadGameEntryObject->SavesMenuData.TotalPlayTimeInSeconds - (Hours * SecondsInHour)) / 60.f;

	const FString& MinutesString = RestMinutes < 10 ? "0" + FString::FromInt(RestMinutes) : FString::FromInt(RestMinutes);

	const FText& TotalTime = FText::FromString(SavedTotalTimeText + FString::FromInt(Hours) + "H " + MinutesString + "MIN" + "-");

	TotalTimeText->SetText(TotalTime);
}

void ULoadSaveEntry::SetScreenshotImageFromSave()
{
	TObjectPtr<UTexture2D> ScreenshotFromSaveGame = UKismetRenderingLibrary::ImportFileAsTexture2D(GetWorld(), LoadGameEntryObject->SavesMenuData.ScreenshotPathSave);
	if (!IsValid(ScreenshotFromSaveGame))
		return;

	ScreenshotSaveImage->SetBrushFromTexture(ScreenshotFromSaveGame);
}