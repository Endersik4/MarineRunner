// Copyright Adam Bartela.All Rights Reserved

#include "InventoryComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/MessageHandlerComponent.h"
#include "WeaponInventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Inventory Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}

	TransformItemsDataToInventory();
	bSpawnNewRecipeUnlockedWidget = true;
}

// Only add starting items when its new game
void UInventoryComponent::TransformItemsDataToInventory()
{
	// if there is something in inventory then dont spawn initial inventory
	if (Inventory_Items.Num() != 0)
		return;

	for (const TPair<FName, int32> CurrentPair : RowNameForStartingItems)
	{
		AddItemToInventory(CurrentPair.Key, CurrentPair.Value);
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

bool UInventoryComponent::AddItemToInventory(FName ItemRowNameFromDataTable, const float& AddAmountToItem)
{
	const FItemStruct* ItemFromDataTable = ItemsDataTable->FindRow<FItemStruct>(ItemRowNameFromDataTable, "");
	if (!ItemFromDataTable)
		return false;

	if (AddAmountToItemIfFound(Inventory_Items.FindByKey(ItemFromDataTable->Item_Name), AddAmountToItem))
		return true;

	// inventory is full
	if (Inventory_Items.Num() > GetMaxSlotsInInventory())
		return false;

	// if weapon slots are full then player cant pick up a new weapon
	if (ItemFromDataTable->bIsItWeapon && IsValid(Player))
	{
		if (!Player->GetWeaponInventoryComponent()->CanPlayerTakeWeaponToInventory())
		{
			Player->GetMessageHandlerComponent()->SpawnNotEnoughSlotsForWeaponWidget();
			return false;
		}
	}

	AddCraftRecipeIfCraftable(ItemFromDataTable);

	FItemStruct ItemWithAmount = *ItemFromDataTable;
	ItemWithAmount.Item_Amount = AddAmountToItem;
	Inventory_Items.Add(ItemWithAmount);

	UpdateInventoryPlayerInformation(ItemWithAmount.bIsItResource);

	return true;
}

bool UInventoryComponent::AddAmountToItemIfFound(FItemStruct* ItemFromInventory, const float& AmountToAdd)
{
	// item was not found in inventory
	if (!ItemFromInventory)
		return false;

	// item slot amount is full
	if (ItemFromInventory->MaxItem_Amount != 0 && ItemFromInventory->Item_Amount >= ItemFromInventory->MaxItem_Amount)
		return false;

	ItemFromInventory->Item_Amount += AmountToAdd;
	UpdateInventoryPlayerInformation(ItemFromInventory->bIsItResource);

	return true;
}

void UInventoryComponent::AddCraftRecipeIfCraftable(const FItemStruct* ItemDataFromDataTable)
{
	if (!ItemDataFromDataTable->bIsItCraftable)
		return;

	// if there is craft recipe of this item then dont add another one
	if (Items_Recipes.FindByKey(*ItemDataFromDataTable))
		return;

	Items_Recipes.Add(*ItemDataFromDataTable);

	if (IsValid(Player) && bSpawnNewRecipeUnlockedWidget)
		Player->GetMessageHandlerComponent()->SpawnNewRecipeUnlockedWidget();
}

void UInventoryComponent::UpdateInventoryPlayerInformation(const bool& bIsItemCraftable)
{
	if (!IsValid(Player))
		return;

	Player->UpdateHudWidget();
	Player->UpdateAlbertosInventory(true, bIsItemCraftable);
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
	Inventory_Items.RemoveSingle(ItemToDelete);
}

void UInventoryComponent::DeleteItemFromInventory(const FName& ItemRowNameToDelete)
{
	FItemStruct* WeaponInformation = GetItemInformationFromDataTable(ItemRowNameToDelete);
	if (!WeaponInformation)
		return;

	DeleteItemFromInventory(*WeaponInformation);
}