// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FItemStruct 
{
	GENERATED_USTRUCT_BODY(); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FString Item_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 Item_Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		UTexture2D* Item_StorageIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool bIsItResource = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FString Item_Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool bIsItWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool bIsItCraftable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		TSubclassOf<class APickupItem> ItemObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		float Item_TimeCraft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FVector Item_CraftLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FVector Item_CraftScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (EditCondition = "bIsItCraftable", EditConditionHides))
		FRotator Item_CraftRotation;
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
		Item_CraftLocation = FVector(0.f);
		Item_CraftScale = FVector(0.f);
		Item_CraftRotation = FRotator(0.f);
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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Inventory", BlueprintReadWrite)
		TMap<TSubclassOf<class APickupItem>, int32> Inventory_ItemsData;

	UPROPERTY(EditAnywhere, Category = "Inventory", BlueprintReadOnly)
		TArray<FItemStruct> Items_Recipes;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", BlueprintReadOnly)
		TMap<FString, FItemStruct> Inventory_Items;


private:

	void TransformItemsDataToInventory();
};
