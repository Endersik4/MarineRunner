// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"
#include "SavedDataObject.generated.h"

/// <summary>
/// Object that saves other object data (for example chest or door was open - save it). Should be only one in the level.
/// </summary>
UCLASS()
class MARINERUNNER_API ASavedDataObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASavedDataObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void AddCustomSaveData(const int32& SavedCustomDataKey, const FCustomDataSaved& SavedCustomData);
	void RemoveCustomSaveData(const int32& SavedCustomDataID);

	FORCEINLINE const TMap<int32, FCustomDataSaved> & GetCustomSavedData() const { return CustomSavedData; }
	int32 CreateUniqueIDForObject() const;

	FORCEINLINE void SetCustomSavedData(TMap<int32, FCustomDataSaved>& NewSavedData) { CustomSavedData = NewSavedData; }

	void LoadObjectsData();
	void StartLoadingObjects();
	void UpdateObjectsData();
	void RestartObjectsData(bool bOnlyDeleteFromTemp = false);
private:
	UPROPERTY(EditAnywhere)
		TMap<int32, FCustomDataSaved> CustomSavedData;
	UPROPERTY(EditAnywhere)
		TMap<int32, FCustomDataSaved> TempCustomSavedData;
	UPROPERTY(EditAnywhere)
		FFloatRange RandomDataIDRange = FFloatRange(1.f, 1000.f);
};
