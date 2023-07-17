// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Inventory/InventoryComponent.h"

#include "MarineRunner/Inventory/PickupItem.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	TransformItemsDataToInventory();
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::TransformItemsDataToInventory()
{
	Inventory_Items.Empty();

	TArray<TSubclassOf<APickupItem>> ItemsDataKeys;
	Inventory_ItemsData.GenerateKeyArray(ItemsDataKeys);
	for (int i = 0; i != Inventory_ItemsData.Num(); i++)
	{
		APickupItem* SpawnedItem = GetWorld()->SpawnActor<APickupItem>(ItemsDataKeys[i], FVector(0.f), FRotator(0.f));
		if (SpawnedItem == nullptr) continue;

		FItemStruct NewItemSettings = SpawnedItem->GetItemSettings();
		NewItemSettings.Item_Amount = *Inventory_ItemsData.Find(ItemsDataKeys[i]);
		Inventory_Items.Add(SpawnedItem->GetItemName(), NewItemSettings);
		SpawnedItem->Destroy();
	}
}