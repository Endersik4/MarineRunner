// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/SavedDataObject.h"


// Sets default values
ASavedDataObject::ASavedDataObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASavedDataObject::BeginPlay()
{
	Super::BeginPlay();
}

void ASavedDataObject::AddCustomSaveData(const int32& SavedCustomDataKey, const FCustomDataSaved& SavedCustomData)
{
	CustomSavedData.Add(SavedCustomDataKey, SavedCustomData);
}

void ASavedDataObject::RemoveCustomSaveData(const int32& SavedCustomDataID)
{
	CustomSavedData.Remove(SavedCustomDataID);
}

void ASavedDataObject::LoadObjectsData()
{
	for (const TPair<int32, FCustomDataSaved>& Pair : CustomSavedData)
	{
		if (Pair.Value.SavedDataState == ESavedDataState::ESDS_SpawnObject)
		{
			if (Pair.Value.ObjectToSpawnFromClass == NULL)
				continue;

			ISaveCustomDataInterface* SpawnedItem = GetWorld()->SpawnActor<ISaveCustomDataInterface>(Pair.Value.ObjectToSpawnFromClass, Pair.Value.ObjectTransform);
			if (SpawnedItem == nullptr)
				continue;

			SpawnedItem->LoadData(Pair.Key, Pair.Value);
			continue;
		}

		if (IsValid(Pair.Value.ObjectToSaveData) == false)
			continue;

		ISaveCustomDataInterface* ActorWithSaveInterface = Cast<ISaveCustomDataInterface>(Pair.Value.ObjectToSaveData);
		if (ActorWithSaveInterface == nullptr)
			continue;

		ActorWithSaveInterface->LoadData(Pair.Key, Pair.Value);
	}
}

void ASavedDataObject::UpdateObjectsData()
{
	for (const TPair<int32, FCustomDataSaved>& Pair : CustomSavedData)
	{
		if (IsValid(Pair.Value.ObjectToSaveData) == false)
			continue;
	
		ISaveCustomDataInterface* ActorWithSaveInterface = Cast<ISaveCustomDataInterface>(Pair.Value.ObjectToSaveData);
		if (ActorWithSaveInterface == nullptr)
			continue;
		
		ActorWithSaveInterface->SaveData(this, Pair.Key, Pair.Value);
	}
}

int32 ASavedDataObject::CreateUniqueIDForObject() const
{
	return CustomSavedData.Num() + (int)FMath::RandRange(RandomDataIDRange.GetLowerBoundValue(), RandomDataIDRange.GetUpperBoundValue());
}