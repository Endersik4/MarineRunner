// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/StorageListEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "MarineRunner/AlbertosClasses/ItemDataObject.h"

void UStorageListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStorageListEntryWidget::NativeOnInitialized()
{
	StorageItemButton->OnHovered.AddDynamic(this, &UStorageListEntryWidget::StorageItemButtonHovered);
	StorageItemButton->OnUnhovered.AddDynamic(this, &UStorageListEntryWidget::StorageItemButtonUnhovered);
}

void UStorageListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UItemDataObject* ItemDataObject = Cast<UItemDataObject>(ListItemObject);
	if (IsValid(ItemDataObject) == false)
		return;

	ItemDataToEntry(ItemDataObject);
}

void UStorageListEntryWidget::ItemDataToEntry(UItemDataObject* ItemDataObject)
{
	ItemImageListEntry->SetBrushFromTexture(ItemDataObject->ItemData.Item_StorageIcon);

	FString ItemAmountValueString = "";
	if (ItemDataObject->ItemData.Item_Amount > 999)
	{
		ItemAmountValueString = TextWhenItemAmountIsMoreThen999;
	}
	else
		ItemAmountValueString = FString::FromInt(ItemDataObject->ItemData.Item_Amount);
	ItemAmountListEntry->SetText(FText::FromString(ItemAmountValueString));

	bNotEnoughResources = ItemDataObject->bIsItEnoughToCraft;
	if (bNotEnoughResources == false)
	{
		PlayAnimationForward(DisableStorageAnim);
	}
	else
	{
		PlayAnimation(StorageItemButtonHoveredAnim, 1.f, 1, EUMGSequencePlayMode::Reverse);
	}
}

void UStorageListEntryWidget::StorageItemButtonHovered()
{
	if (bNotEnoughResources == false)
	{
		PlayAnimationForward(DisabledStorageItemButtonHoveredAnim);
	}
	else
	{
		PlayAnimationForward(StorageItemButtonHoveredAnim);
	}
}

void UStorageListEntryWidget::StorageItemButtonUnhovered()
{
	if (bNotEnoughResources == false)
	{
		PlayAnimationReverse(DisabledStorageItemButtonHoveredAnim);
	}
	else
	{
		PlayAnimationReverse(StorageItemButtonHoveredAnim);
	}
}
