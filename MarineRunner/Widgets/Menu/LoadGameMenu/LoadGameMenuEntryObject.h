// Fill out your copyright notice in the Description page of Project Settings.

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

	bool operator<(const ULoadGameMenuEntryObject&);
};
