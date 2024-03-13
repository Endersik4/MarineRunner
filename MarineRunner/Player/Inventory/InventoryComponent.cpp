// Copyright Adam Bartela.All Rights Reserved

#include "InventoryComponent.h"

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

// Only add starting items when its new game
void UInventoryComponent::TransformItemsDataToInventory()
{
	if (Inventory_Items.Num() != 0)
		return;

	for (const TPair<FName, int32> CurrentPair : RowNameForStartingItems)
	{
		AddNewItemToInventory(CurrentPair.Key, CurrentPair.Value);
	}
}

FItemStruct* UInventoryComponent::GetItemFromInventory(FName ItemRowNameFromDataTable)
{
	const FItemStruct* ItemFromDataTable = ItemsDataTable->FindRow<FItemStruct>(ItemRowNameFromDataTable, "");
	if (!ItemFromDataTable)
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
	if (!ItemFromDataTable)
		return;

	FItemStruct ItemWithAmount = *ItemFromDataTable;

	if (ItemWithAmount.bIsItCraftable)
		Items_Recipes.Add(ItemWithAmount);

	ItemWithAmount.Item_Amount = AddAmountToItem;
	Inventory_Items.Add(ItemWithAmount);
}

void UInventoryComponent::MoveWeaponRecipesToEndQueue()
{
	TArray<FItemStruct> WeaponRecipes;
	for (const FItemStruct& CurrentItem : Items_Recipes)
	{
		if (CurrentItem.bIsItWeapon == true)
			WeaponRecipes.Add(CurrentItem);
	}

	for (const FItemStruct& CurrentWeapon : WeaponRecipes)
	{
		Items_Recipes.Remove(CurrentWeapon);
		Items_Recipes.Add(CurrentWeapon);
	}
}

void UInventoryComponent::DeleteItemFromInventory(FItemStruct ItemToDelete)
{
	Inventory_Items.Remove(ItemToDelete);
}