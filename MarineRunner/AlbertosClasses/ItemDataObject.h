// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "ItemDataObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UItemDataObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FItemStruct ItemData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsItEnoughToCraft = true;
};
