// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuWidget.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"

#include "LoadGameMenuEntryObject.h"
#include "LoadGameData.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

void ULoadGameMenuWidget::NativeConstruct()
{
}

void ULoadGameMenuWidget::NativeOnInitialized()
{
	FillSavesListView();
}

void ULoadGameMenuWidget::FillSavesListView()
{
	AMarineCharacter* PlayerCharacter = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (IsValid(PlayerCharacter) == false) return;
	if (PlayerCharacter->GetPointerToSaveMenuDataFromSave() == nullptr) return;

	TArray<FSaveDataMenuStruct> SaveMenuData = *PlayerCharacter->GetPointerToSaveMenuDataFromSave();

	SavesListView->ClearListItems();

	for (const FSaveDataMenuStruct& CurrentSaveData : SaveMenuData)
	{
		ULoadGameMenuEntryObject* ConstructedItemObject = NewObject<ULoadGameMenuEntryObject>(MenuSettingsDataObject);
		if (IsValid(ConstructedItemObject) == false) continue;

		ConstructedItemObject->SavesMenuData = CurrentSaveData;

		SavesListView->AddItem(ConstructedItemObject);
	}
}
