// Copyright Adam Bartela.All Rights Reserved


#include "LoadGameMenuWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SaveToLoadEntryObject.h"
#include "MarineRunner/Player/SaveLoadGame/JsonFileActions.h"

void ULoadGameMenuWidget::NativeOnInitialized()
{
	FillLoadGamesListView();
}

void ULoadGameMenuWidget::FillLoadGamesListView()
{
	LoadGamesListView->ClearListItems();

	TArray<FString> Txt_Files;
	GetTextFilesFromSaves(Txt_Files);

	if (Txt_Files.Num() <= 0)
		return;

	NoSavedDataText->SetVisibility(ESlateVisibility::Hidden);

	TArray<FSaveDataMenuStruct> DeserlializedSaves;

	FillDeserializedSaveFilesToArray(Txt_Files, DeserlializedSaves);

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
			return;

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
		TObjectPtr<ULoadGameMenuEntryObject> ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);

		if (!IsValid(ConstructedItemObject))
			continue;

		ConstructedItemObject->SavesMenuData = CurrentSaveDataMenu;

		LoadGamesListView->AddItem(ConstructedItemObject);
	}
}