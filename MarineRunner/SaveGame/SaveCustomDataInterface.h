// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveCustomDataInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveCustomDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MARINERUNNER_API ISaveCustomDataInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
		virtual void LoadData(int32 StateOfData = 0) = 0;
};

USTRUCT(BlueprintType)
struct FCustomDataSaved
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		TScriptInterface<ISaveCustomDataInterface> ObjectToSaveData;

	UPROPERTY(EditAnywhere)
		int32 StateOfSave;

	FCustomDataSaved()
	{
		ObjectToSaveData = nullptr;
		StateOfSave = 0;
	}

	FCustomDataSaved(TScriptInterface<ISaveCustomDataInterface> _ObjectToSaveData, int32 _StateOfSave)
	{
		ObjectToSaveData = _ObjectToSaveData;
		StateOfSave = _StateOfSave;
	}

	FCustomDataSaved& operator=(const FCustomDataSaved& OtherObject)
	{
		ObjectToSaveData = OtherObject.ObjectToSaveData;
		StateOfSave = OtherObject.StateOfSave;
		return *this;
	}

	bool operator==(const TScriptInterface<ISaveCustomDataInterface> OtherObject)
	{
		return ObjectToSaveData == OtherObject;
	}

	bool operator==(const FCustomDataSaved& OtherObject)
	{
		return ObjectToSaveData == OtherObject.ObjectToSaveData;
	}
};