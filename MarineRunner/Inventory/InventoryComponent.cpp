// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Inventory/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	TransformItemsDataToInventory();
}

void UInventoryComponent::TransformItemsDataToInventory()
{
	TArray<FName> ItemName;
	RowNameForItem.GenerateKeyArray(ItemName);
	for (const FName& CurrentItemName : ItemName)
	{
		AddNewItemToInventory(CurrentItemName , *RowNameForItem.Find(CurrentItemName));
	}
	ItemName.Empty();
}

FItemStruct* UInventoryComponent::GetItemFromInventory(FName ItemRowNameFromDataTable)
{
	const FItemStruct* ItemFromDataTable = ItemsDataTable->FindRow<FItemStruct>(ItemRowNameFromDataTable, "");
	if (ItemFromDataTable == nullptr)
		return nullptr;

	return Inventory_Items.FindByKey(ItemFromDataTable->Item_Name);
}

FItemStruct* UInventoryComponent::GetItemInformationFromDataTable(FName ItemRowNameFromDataTable)
{
	return ItemsDataTable->FindRow<FItemStruct>(ItemRowNameFromDataTable, "");
}

void UInventoryComponent::AddNewItemToInventory(FName ItemRowNameFromDataTable, float AddAmountToItem)
{
	const FItemStruct* ItemFromDataTable = ItemsDataTable->FindRow<FItemStruct>(ItemRowNameFromDataTable, "");
	if (ItemFromDataTable == nullptr)
		return;

	FItemStruct ItemWithAmount = *ItemFromDataTable;
	ItemWithAmount.Item_Amount = AddAmountToItem;

	if (ItemWithAmount.bIsItCraftable)
		Items_Recipes.Add(ItemWithAmount);

	Inventory_Items.Add(ItemWithAmount);
}

void UInventoryComponent::DeleteItemFromInventory(FItemStruct ItemToDelete)
{
	Inventory_Items.Remove(ItemToDelete);
}