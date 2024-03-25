// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Widgets/Crafting/InventorySlotEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "MarineRunner/Albertos/Objects/CraftedItemDataObject.h"

void UInventorySlotEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySlotEntryWidget::NativeOnInitialized()
{
	if (!IsValid(ItemButton))
		return;

	ItemButton->OnHovered.AddDynamic(this, &UInventorySlotEntryWidget::ItemButtonHovered);
	ItemButton->OnUnhovered.AddDynamic(this, &UInventorySlotEntryWidget::ItemButtonUnhovered);
}

void UInventorySlotEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!IsValid(ListItemObject))
		return;

	TObjectPtr<UCraftedItemDataObject> ItemDataObject = Cast<UCraftedItemDataObject>(ListItemObject);
	if (!IsValid(ItemDataObject))
		return;

	ItemDataToUI(ItemDataObject);
}

void UInventorySlotEntryWidget::ItemDataToUI(TObjectPtr<UCraftedItemDataObject> ItemDataObject)
{
	ItemImage->SetBrushFromTexture(ItemDataObject->ItemData.Item_StorageIcon);

	const FString& ItemAmountValueString = ItemDataObject->ItemData.Item_Amount > MaxAmountToDisplay ? ExceededMaxAmountToDisplayText : FString::FromInt(ItemDataObject->ItemData.Item_Amount);
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

void UInventorySlotEntryWidget::ItemButtonHovered()
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

void UInventorySlotEntryWidget::ItemButtonUnhovered()
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
