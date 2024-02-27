// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/SavedDataObject.h"

ASavedDataObject::ASavedDataObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASavedDataObject::BeginPlay()
{
	Super::BeginPlay();
}

void ASavedDataObject::AddCustomSaveData(const int32& SavedCustomDataKey, const FCustomDataSaved& SavedCustomData)
{
	RemoveCustomSaveData(SavedCustomDataKey);
	CustomSavedData.Add(SavedCustomDataKey, SavedCustomData);
	TempCustomSavedData.Add(SavedCustomDataKey, SavedCustomData);
}

void ASavedDataObject::RemoveCustomSaveData(const int32& SavedCustomDataID)
{
	CustomSavedData.Remove(SavedCustomDataID);
	//TempCustomSavedData.Remove(SavedCustomDataID);
}

void ASavedDataObject::StartLoadingObjects()
{
	FTimerHandle test;
	GetWorld()->GetTimerManager().SetTimer(test, this, &ASavedDataObject::LoadObjectsData, 0.05f, false);
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

		if (Pair.Value.bValueNotSavedWhileInGame == true)
		{
			TempCustomSavedData.Add(Pair);
		}
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

void ASavedDataObject::RestartObjectsData()
{
	if (TempCustomSavedData.Num() == 0)
		return;

	TArray<int32> KeysToRemoveFromCustomSavedData;

	for (const TPair<int32, FCustomDataSaved>& Pair : TempCustomSavedData)
	{
		try 
		{	
			if (IsValid(Pair.Value.ObjectToSaveData) == false)
				continue;
		}
		catch(...)
		{
			UE_LOG(LogTemp, Error, TEXT("NIE DZIALA"));
			continue;
		}

		ISaveCustomDataInterface* ActorWithSaveInterface = Cast<ISaveCustomDataInterface>(Pair.Value.ObjectToSaveData);
		if (ActorWithSaveInterface == nullptr)
			continue;

		ActorWithSaveInterface->RestartData(this, Pair.Key, Pair.Value);

		if (Pair.Value.bValueNotSavedWhileInGame == false)
		{
			KeysToRemoveFromCustomSavedData.Add(Pair.Key);
		}
	}

	for (const int32& CurrentKeyToRemove : KeysToRemoveFromCustomSavedData)
	{
		CustomSavedData.Remove(CurrentKeyToRemove);
		TempCustomSavedData.Remove(CurrentKeyToRemove);
	}
}

int32 ASavedDataObject::CreateUniqueIDForObject() const
{
	return CustomSavedData.Num() + (int)FMath::RandRange(RandomDataIDRange.GetLowerBoundValue(), RandomDataIDRange.GetUpperBoundValue());
}