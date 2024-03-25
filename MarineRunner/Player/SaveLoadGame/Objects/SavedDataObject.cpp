// Copyright Adam Bartela.All Rights Reserved

#include "SavedDataObject.h"

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
}

void ASavedDataObject::StartLoadingObjects()
{
	FTimerHandle LoadingObjectsDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(LoadingObjectsDelayHandle, this, &ASavedDataObject::LoadObjectsData, StartLoadObjectsDataTime, false);
}

void ASavedDataObject::LoadObjectsData()
{
	for (const TPair<int32, FCustomDataSaved>& Pair : CustomSavedData)
	{
		if (Pair.Value.SavedDataState == ESavedDataState::ESDS_SpawnObject)
		{
			if (!IsValid(Pair.Value.ObjectToSpawnFromClass))
				continue;

			ISaveCustomDataInterface* SpawnedItem = GetWorld()->SpawnActor<ISaveCustomDataInterface>(Pair.Value.ObjectToSpawnFromClass, Pair.Value.ObjectTransform);
			if (!SpawnedItem)
				continue;

			SpawnedItem->LoadData(Pair.Key, Pair.Value);
			continue;
		}

		if (!IsValid(Pair.Value.ObjectToSaveData))
			continue;

		ISaveCustomDataInterface* ActorWithSaveInterface = Cast<ISaveCustomDataInterface>(Pair.Value.ObjectToSaveData);
		if (!ActorWithSaveInterface)
			continue;

		ActorWithSaveInterface->LoadData(Pair.Key, Pair.Value);
	}
}

void ASavedDataObject::UpdateObjectsData()
{
	for (const TPair<int32, FCustomDataSaved>& Pair : CustomSavedData)
	{
		if (!ensureMsgf(IsValid(Pair.Value.ObjectToSaveData), TEXT("Object To update data is nullptr")))
			continue;

		ISaveCustomDataInterface* ActorWithSaveInterface = Cast<ISaveCustomDataInterface>(Pair.Value.ObjectToSaveData);
		if (!ActorWithSaveInterface)
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
		if (!Pair.Value.ObjectToSaveData)
			continue;

		if (!IsValid(Pair.Value.ObjectToSaveData))
			continue;

		ISaveCustomDataInterface* ActorWithSaveInterface = Cast<ISaveCustomDataInterface>(Pair.Value.ObjectToSaveData);
		if (!ActorWithSaveInterface)
			continue;

		ActorWithSaveInterface->RestartData(this, Pair.Key, Pair.Value);

		if (!Pair.Value.bValueSavedFromTheBeginning)
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

void ASavedDataObject::RemoveTempCustomSavedData()
{
	if (TempCustomSavedData.Num() == 0)
		return;

	TArray<int32> KeysToRemoveFromCustomSavedData;
	for (const TPair<int32, FCustomDataSaved>& Pair : TempCustomSavedData)
	{
		if (Pair.Value.bValueSavedFromTheBeginning)
			continue;

		KeysToRemoveFromCustomSavedData.Add(Pair.Key);
	}

	for (const int32& CurrentKeyToRemove : KeysToRemoveFromCustomSavedData)
	{
		TempCustomSavedData.Remove(CurrentKeyToRemove);
	}
}

int32 ASavedDataObject::CreateUniqueIDForObject() const
{
	return CustomSavedData.Num() + (int)FMath::RandRange(RandomDataIDRangeForObject.GetLowerBoundValue(), RandomDataIDRangeForObject.GetUpperBoundValue());
}