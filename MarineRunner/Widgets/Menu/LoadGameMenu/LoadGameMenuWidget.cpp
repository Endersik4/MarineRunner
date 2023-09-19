// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "Components/ListView.h"

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

	for (const FSaveDataMenuStruct& CurrentSaveData : SavesData)
	{
		ULoadGameMenuEntryObject* ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);
		if (IsValid(ConstructedItemObject) == false) continue;

		ConstructedItemObject->SavesMenuData = CurrentSaveData;

		SavesListView->AddItem(ConstructedItemObject);
	}
}
