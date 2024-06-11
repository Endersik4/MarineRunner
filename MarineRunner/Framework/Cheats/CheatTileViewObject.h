// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CheatWidget.h"
#include "CheatTileViewObject.generated.h"

UCLASS()
class MARINERUNNER_API UCheatTileViewObject : public UObject
{
	GENERATED_BODY()
	
public:
	FCheatData EntryCheatData;
};
