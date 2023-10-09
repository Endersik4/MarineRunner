// Fill out your copyright notice in the Description page of Project Settings.


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
	Txt_Files.Sort();

	if (Txt_Files.Num() > 0)
		NoSavedDataText->SetVisibility(ESlateVisibility::Hidden);

	for (const FString& CurrTxtFilePath : Txt_Files)
	{
		FSaveDataMenuStruct NewSaveDataMenu;

		TSharedPtr<FJsonObject> JsonObject;
		bool bWasJsonDeserialize = USaveGameJsonFile::ReadJson(CurrTxtFilePath, JsonObject);
		if (bWasJsonDeserialize)
		{
			NewSaveDataMenu.SaveName = JsonObject->GetStringField("SavedDataName");
			NewSaveDataMenu.SaveNumber = JsonObject->GetNumberField("SavedDataNumber");
			NewSaveDataMenu.ScreenshotPathSave = JsonObject->GetStringField("PathToThumbnail");
			NewSaveDataMenu.SaveDateTime = JsonObject->GetStringField("SavedGameDate");
			NewSaveDataMenu.LevelNameToLoad = JsonObject->GetStringField("SavedLevelName");
			NewSaveDataMenu.TotalPlayTimeInSeconds = JsonObject->GetNumberField("TotalPlayTime");
		}
		else return;
		
		ULoadGameMenuEntryObject* ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);
		if (IsValid(ConstructedItemObject) == false) continue;

		ConstructedItemObject->SavesMenuData = NewSaveDataMenu;

		TArray<UObject*> ListItems = SavesListView->GetListItems();
		ListItems.Insert(ConstructedItemObject, 0);
		SavesListView->SetListItems(ListItems);
	}
}
