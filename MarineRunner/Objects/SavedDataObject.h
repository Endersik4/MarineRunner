// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavedDataObject.generated.h"

/// <summary>
/// Object that saves other object data (for example chest or door was open - save it). Should be only one in the level.
/// </summary>
class ADoor;
class AChestWithItems;
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void AddSavedDoor(ADoor* SaveDoor) { SavedDoor.AddUnique(SaveDoor); }
	FORCEINLINE void AddSavedChests(AChestWithItems* SaveChest) { SavedChests.AddUnique(SaveChest); }

	FORCEINLINE TArray<ADoor*> GetSavedDoor() const { return SavedDoor; }
	FORCEINLINE TArray<AChestWithItems*> GetSavedChests() const { return SavedChests; }

	void LoadObjectsData();

private:
	TArray<ADoor*> SavedDoor;
	TArray<AChestWithItems*> SavedChests;


};
