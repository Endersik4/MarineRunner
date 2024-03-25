// Copyright Adam Bartela.All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveInformationData.h"

#include "SaveToLoadEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API ULoadGameMenuEntryObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		FSaveDataMenuStruct SavesMenuData = FSaveDataMenuStruct();
};
