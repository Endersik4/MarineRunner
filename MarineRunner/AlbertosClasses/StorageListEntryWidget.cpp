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

	ItemImageListEntry->SetBrushFromTexture(ItemDataObject->ItemData.Item_StorageIcon);

	FString ItemAmountValueString = FString::FromInt(ItemDataObject->ItemData.Item_Amount);
	if (ItemDataObject->ItemData.Item_Amount > 999)
	{
		ItemAmountValueString = TextWhenItemAmountIsMoreThen999;
	}
	ItemAmountListEntry->SetText(FText::FromString(ItemAmountValueString));
	
	if (ItemDataObject->bIsItEnoughToCraft == false)
	{
		PlayAnimationForward(DisableStorageAnim);
	}
	else
	{
		PlayAnimationReverse(DisableStorageAnim);
	}
}

void UStorageListEntryWidget::StorageItemButtonHovered()
{
	if (StorageItemButtonHoveredAnim == nullptr)
		return;

	PlayAnimationForward(StorageItemButtonHoveredAnim);
}

void UStorageListEntryWidget::StorageItemButtonUnhovered()
{
	if (StorageItemButtonHoveredAnim == nullptr)
		return;

	PlayAnimationReverse(StorageItemButtonHoveredAnim);
}
