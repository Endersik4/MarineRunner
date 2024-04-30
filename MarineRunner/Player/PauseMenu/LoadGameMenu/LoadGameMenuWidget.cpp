// Copyright Adam Bartela.All Rights Reserved


#include "LoadGameMenuWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HAL/FileManagerGeneric.h"
#include "Kismet/GameplayStatics.h"

#include "SaveToLoadEntryObject.h"
#include "MarineRunner/Player/SaveLoadGame/JsonFileActions.h"
#include "MarineRunner/Player/PauseMenu/ConfirmOptionWidget.h"


void ULoadGameMenuWidget::NativeOnInitialized()
{
	FillAllSavesToLoadGameListView();

	DeleteAllSavesButton->OnClicked.AddDynamic(this, &ULoadGameMenuWidget::DeleteAllSaves_OnClicked);
	DeleteAllSavesButton->OnHovered.AddDynamic(this, &ULoadGameMenuWidget::DeleteAllSaves_OnHovered);
	DeleteAllSavesButton->OnUnhovered.AddDynamic(this, &ULoadGameMenuWidget::DeleteAllSaves_OnUnhovered);
}

void ULoadGameMenuWidget::FillAllSavesToLoadGameListView()
{
	SavedGameSavesListView->ClearListItems();

	TArray<FString> JsonFilesOfAllSavedSaves;
	FFileManagerGeneric::Get().FindFilesRecursive(JsonFilesOfAllSavedSaves, *(UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/"), *FString("*.json"), true, false);

	if (JsonFilesOfAllSavedSaves.Num() <= 0)
		return;
	
	SavesVisible();

	TArray<FSaveDataMenuStruct> DeserlializedSaves;
	FillDeserializedSaveFilesToArray(JsonFilesOfAllSavedSaves, DeserlializedSaves);

	DeserlializedSaves.Sort([](const FSaveDataMenuStruct& a, const FSaveDataMenuStruct& b) { return a.SavedDateValue > b.SavedDateValue; });

	ConvertArrayToLoadGameMenuEntryList(DeserlializedSaves);
}

void ULoadGameMenuWidget::FillDeserializedSaveFilesToArray(TArray<FString>& PathsToSaveFiles, TArray<FSaveDataMenuStruct>& ArrayToFill)
{
	for (const FString& CurrTxtFilePath : PathsToSaveFiles)
	{
		TSharedPtr<FJsonObject> JsonObject;
		const bool bWasJsonDeserialize = USaveGameJsonFile::ReadJson(CurrTxtFilePath, JsonObject);
		if (!bWasJsonDeserialize)
			continue;

		FSaveDataMenuStruct NewSaveDataMenu;
		NewSaveDataMenu.SaveName = JsonObject->GetStringField("SavedDataName");
		NewSaveDataMenu.SaveNumber = JsonObject->GetIntegerField("SavedDataNumber");
		NewSaveDataMenu.ScreenshotPathSave = JsonObject->GetStringField("PathToThumbnail");
		NewSaveDataMenu.SaveDateTime = JsonObject->GetStringField("SavedGameDate");
		NewSaveDataMenu.LevelNameToLoad = JsonObject->GetStringField("SavedLevelName");
		NewSaveDataMenu.TotalPlayTimeInSeconds = JsonObject->GetNumberField("TotalPlayTime");
		NewSaveDataMenu.SavedDateValue = JsonObject->GetIntegerField("SavedDateValue");

		ArrayToFill.Add(NewSaveDataMenu);
	}
}

void ULoadGameMenuWidget::ConvertArrayToLoadGameMenuEntryList(TArray<FSaveDataMenuStruct>& ArrayToConvert)
{
	for (const FSaveDataMenuStruct& CurrentSaveDataMenu : ArrayToConvert)
	{
		TObjectPtr<ULoadGameMenuEntryObject> ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(SavedGameSaveEntry);

		if (!IsValid(ConstructedItemObject))
			continue;

		ConstructedItemObject->SavesMenuData = CurrentSaveDataMenu;

		SavedGameSavesListView->AddItem(ConstructedItemObject);
	}
}

void ULoadGameMenuWidget::DeleteAllSaves_OnClicked()
{
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PlayerController))
		return;

	TObjectPtr<UConfirmOptionWidget> ConfirmLoadingWidget = Cast<UConfirmOptionWidget>(CreateWidget(PlayerController, ConfirmDeletingSavesWidgetClass));
	if (!IsValid(ConfirmLoadingWidget))
		return;

	ConfirmLoadingWidget->AddToViewport();
	ConfirmLoadingWidget->ConfirmFunction = [this]() {this->DeleteAllSaves(); };
}

void ULoadGameMenuWidget::DeleteAllSaves()
{
	SavesVisible(true);

	FFileManagerGeneric::Get().DeleteDirectory(*(UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/"), true, true);
}

void ULoadGameMenuWidget::DeleteAllSaves_OnHovered()
{
	PlayAnimationForward(DeleteAllSavesHoveredAnim);
}

void ULoadGameMenuWidget::DeleteAllSaves_OnUnhovered()
{
	PlayAnimationReverse(DeleteAllSavesHoveredAnim);
}

void ULoadGameMenuWidget::SavesVisible(bool bHide)
{
	NoSavedDataText->SetVisibility(bHide ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	SavedGameSavesListView->SetVisibility(bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	DeleteAllSavesButton->SetVisibility(bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	DeleteAllSavesText->SetVisibility(bHide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}