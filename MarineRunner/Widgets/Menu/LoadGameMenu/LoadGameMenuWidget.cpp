// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "LoadGameMenuEntryObject.h"
#include "LoadGameData.h"
#include "MarineRunner/SaveGame/SaveGameJsonFile.h"

void ULoadGameMenuWidget::NativeConstruct()
{
}

void ULoadGameMenuWidget::NativeOnInitialized()
{
	FillSavesListView();
}

void ULoadGameMenuWidget::FillSavesListView()
{
	SavesListView->ClearListItems();

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
		FSaveDataMenuStruct NewSaveDataMenu;

		TSharedPtr<FJsonObject> JsonObject;
		bool bWasJsonDeserialize = USaveGameJsonFile::ReadJson(CurrTxtFilePath, JsonObject);
		if (bWasJsonDeserialize == false)
			return;

		NewSaveDataMenu.SaveName = JsonObject->GetStringField("SavedDataName");
		NewSaveDataMenu.SaveNumber = JsonObject->GetNumberField("SavedDataNumber");
		NewSaveDataMenu.ScreenshotPathSave = JsonObject->GetStringField("PathToThumbnail");
		NewSaveDataMenu.SaveDateTime = JsonObject->GetStringField("SavedGameDate");
		NewSaveDataMenu.LevelNameToLoad = JsonObject->GetStringField("SavedLevelName");
		NewSaveDataMenu.TotalPlayTimeInSeconds = JsonObject->GetNumberField("TotalPlayTime");
		NewSaveDataMenu.SavedDateValue = JsonObject->GetNumberField("SavedDateValue");

		ArrayToFill.Add(NewSaveDataMenu);
	}
}

void ULoadGameMenuWidget::ConvertArrayToLoadGameMenuEntryList(TArray<FSaveDataMenuStruct>& ArrayToConvert)
{
	for (const FSaveDataMenuStruct& CurrentSaveDataMenu : ArrayToConvert)
	{
		ULoadGameMenuEntryObject* ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);

		if (IsValid(ConstructedItemObject) == false)
			continue;

		ConstructedItemObject->SavesMenuData = CurrentSaveDataMenu;

		SavesListView->AddItem(ConstructedItemObject);
	}
}