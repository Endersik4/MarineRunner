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

	FItemStruct()
	{
		Item_Name = " ";
		Item_Amount = 0;
	}

	FItemStruct(FString Name, int32 Amount)
	{
		Item_Name = Name;
		Item_Amount = Amount;
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

	UPROPERTY(EditAnywhere, Category = "Inventory")
		TMap<FString, FItemStruct> Inventory_Items;
};
