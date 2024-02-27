// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FItemStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY(); 

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FString Item_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 Item_Amount;
	// if 0 then unlimited
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 MaxItem_Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		UTexture2D* Item_StorageIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool bIsItResource = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FString Item_Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool bIsItWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition="bIsItWeapon", EditConditionHides))
		TSubclassOf<class AGun> WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool bIsItCraftable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		TSubclassOf<class APickupItem> ItemObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		float Item_TimeCraft;
	// When item was spawned in Location of ItemCraftSocket then add this offset to location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FVector InitialCraftLocationOffset;
	// When player crafted an item then set this scale to the item to fit in albertos. If equals 1 then the scale will not be changed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FVector InitialCraftScale = FVector(1.f);
	// when the crafting of this item ended then scale item to TargetScale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FVector TargetScaleAfterCrafting;
	// When item was spawned in Location of ItemCraftSocket then add this offset to rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FRotator InitialCraftRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		TMap<FString, int32> ResourceRequirements;

	FItemStruct()
	{
		Item_Name = " ";
		Item_Amount = 0;
		Item_StorageIcon = nullptr;
		Item_Description = " ";
		bIsItResource = false;
		bIsItWeapon = false;
		bIsItCraftable = false;
		InitialCraftLocationOffset = FVector(0.f);
		InitialCraftScale = FVector(1.f);
		InitialCraftRotation = FRotator(0.f);
		Item_TimeCraft = 0.f;
		ResourceRequirements = { {" ", 0} };
	}

	FItemStruct(FString Name, int32 Amount, UTexture2D* StorageIcon, FString Description = " ", bool bResource = false, bool bCraftable = false, float CraftingTime = 0.f, TMap<FString, int32> Requirements = { {" ", 0}})
	{
		Item_Name = Name;
		Item_Amount = Amount;
		Item_StorageIcon = StorageIcon;
		Item_Description = Description;
		bIsItResource = bResource;
		bIsItCraftable = bCraftable;
		Item_TimeCraft = CraftingTime;
		ResourceRequirements = Requirements;
	}

	bool operator==(const FItemStruct& SecondItem)
	{
		return Item_Name == SecondItem.Item_Name;
	}

	bool operator==(const FString& SecondItemName)
	{
		return Item_Name == SecondItemName;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, Category = "Inventory", BlueprintReadWrite)
		TMap<FName, int32> RowNameForStartingItems;
	UPROPERTY(EditAnywhere, Category = "Inventory", BlueprintReadWrite)
		UDataTable* ItemsDataTable;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
		TArray<FItemStruct> Inventory_Items;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
		TArray<FItemStruct> Items_Recipes;

	FItemStruct* GetItemFromInventory(FName ItemRowNameFromDataTable);
	FItemStruct* GetItemInformationFromDataTable(FName ItemRowNameFromDataTable);

	void AddNewItemToInventory(FName ItemRowNameFromDataTable, float AddAmountToItem = 0.f);
	void DeleteItemFromInventory(FItemStruct ItemToDelete);
	void MoveWeaponRecipesToEndQueue();

private:

	void TransformItemsDataToInventory();
};
