// Copyright Adam Bartela.All Rights Reserved


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
	ItemButton->OnHovered.AddDynamic(this, &UStorageListEntryWidget::ItemButtonHovered);
	ItemButton->OnUnhovered.AddDynamic(this, &UStorageListEntryWidget::ItemButtonUnhovered);
}

void UStorageListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	TObjectPtr<UItemDataObject> ItemDataObject = Cast<UItemDataObject>(ListItemObject);
	if (!IsValid(ItemDataObject))
		return;

	ItemDataToUI(ItemDataObject);
}

void UStorageListEntryWidget::ItemDataToUI(TObjectPtr<UItemDataObject> ItemDataObject)
{
	ItemImage->SetBrushFromTexture(ItemDataObject->ItemData.Item_StorageIcon);

	FString ItemAmountValueString = "";
	if (ItemDataObject->ItemData.Item_Amount > 999)
	{
		ItemAmountValueString = TextWhenItemAmountIsMoreThen999;
	}
	else
		ItemAmountValueString = FString::FromInt(ItemDataObject->ItemData.Item_Amount);

	ItemAmountTextBlock->SetText(FText::FromString(ItemAmountValueString));

	bNotEnoughResources = ItemDataObject->bIsItEnoughToCraft;
	if (!bNotEnoughResources)
	{
		PlayAnimationForward(DisableItemAnim);
	}
	else
	{
		PlayAnimation(ItemHoveredAnim, 1.f, 1, EUMGSequencePlayMode::Reverse);
	}
}

void UStorageListEntryWidget::ItemButtonHovered()
{
	if (!bNotEnoughResources)
	{
		PlayAnimationForward(DisabledItemHoveredAnim);
	}
	else
	{
		PlayAnimationForward(ItemHoveredAnim);
	}
}

void UStorageListEntryWidget::ItemButtonUnhovered()
{
	if (!bNotEnoughResources)
	{
		PlayAnimationReverse(DisabledItemHoveredAnim);
	}
	else
	{
		PlayAnimationReverse(ItemHoveredAnim);
	}
}
