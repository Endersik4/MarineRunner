// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LoadSaveInformationData.h"

#include "LoadSaveEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API ULoadSaveEntryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FSaveDataMenuStruct SavesMenuData = FSaveDataMenuStruct();
	UPROPERTY(Transient)
	TObjectPtr<class UGameMenuBase> CurrentSpawnedMenu = nullptr;
};
