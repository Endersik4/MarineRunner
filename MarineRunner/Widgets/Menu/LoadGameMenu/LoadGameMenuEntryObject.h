// Copyright Adam Bartela.All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LoadGameData.h"

#include "LoadGameMenuEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API ULoadGameMenuEntryObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		FSaveDataMenuStruct SavesMenuData;
};
