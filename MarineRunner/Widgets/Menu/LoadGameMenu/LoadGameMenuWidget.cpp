// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "LoadGameMenuEntryObject.h"
#include "LoadGameData.h"

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

	if (Txt_Files.Num() > 0)
		NoSavedDataText->SetVisibility(ESlateVisibility::Hidden);

	for (const FString& CurrTxtFilePath : Txt_Files)
	{
		FJsonSerializableArray DataFromFile;
		FFileHelper::LoadFileToStringArray(DataFromFile, *CurrTxtFilePath);
		if (DataFromFile.Num() < 5)
		{
			UE_LOG(LogTemp, Error, TEXT("DATA FROM FILE NUM < 4"));
			continue;
		}

		FSaveDataMenuStruct NewSaveDataMenu = FSaveDataMenuStruct(DataFromFile[0], FCString::Atoi(*DataFromFile[1]), DataFromFile[2], DataFromFile[3], DataFromFile[4], FCString::Atoi(*DataFromFile[5]));

		ULoadGameMenuEntryObject* ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);
		if (IsValid(ConstructedItemObject) == false) continue;

		ConstructedItemObject->SavesMenuData = NewSaveDataMenu;

		TArray<UObject*> ListItems = SavesListView->GetListItems();
		ListItems.Insert(ConstructedItemObject, 0);
		ListItems.Sort();
		SavesListView->SetListItems(ListItems);
	}
}
