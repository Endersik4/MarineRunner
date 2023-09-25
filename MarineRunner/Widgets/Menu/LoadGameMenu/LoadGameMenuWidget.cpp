// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "Components/ListView.h"
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

	for (const FString& CurrTxtFilePath : Txt_Files)
	{
		FJsonSerializableArray DataFromFile;
		FFileHelper::LoadFileToStringArray(DataFromFile, *CurrTxtFilePath);
		if (DataFromFile.Num() < 4)
		{
			UE_LOG(LogTemp, Error, TEXT("DATA FROM FOLE NUM < 4"));
			continue;
		}

		FSaveDataMenuStruct NewSaveDataMenu = FSaveDataMenuStruct(DataFromFile[0], DataFromFile[1], DataFromFile[2], FCString::Atoi(*DataFromFile[3]));

		ULoadGameMenuEntryObject* ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);
		if (IsValid(ConstructedItemObject) == false) continue;

		ConstructedItemObject->SavesMenuData = NewSaveDataMenu;

		TArray<UObject*> ListItems = SavesListView->GetListItems();
		ListItems.Insert(ConstructedItemObject, 0);
		SavesListView->SetListItems(ListItems);
	}
}
