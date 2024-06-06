// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "CraftedItemDataObject.generated.h"

/**
 * Item Data Object to get cast it as item data object in list entry item
 */
UCLASS()
class MARINERUNNER_API UCraftedItemDataObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStruct ItemData = FItemStruct();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsItEnoughToCraft = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWasDisabled = false;
};
