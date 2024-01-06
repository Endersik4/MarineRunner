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

// Called every frame
void ASavedDataObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASavedDataObject::AddCustomSaveData(TScriptInterface<ISaveCustomDataInterface> ObjectToSave, int32 StateOfObjectToSave)
{
	FCustomDataSaved NewCustomSavedData(ObjectToSave, StateOfObjectToSave);
	CustomSavedData.Remove(NewCustomSavedData);
	CustomSavedData.Add(NewCustomSavedData);
}

void ASavedDataObject::LoadObjectsData()
{
	for (FCustomDataSaved & CurrentCustomSavedData : CustomSavedData)
	{
		if (CurrentCustomSavedData.ObjectToSaveData == nullptr)
		{
			continue;
		}

		CurrentCustomSavedData.ObjectToSaveData->LoadData(CurrentCustomSavedData.StateOfSave);
	}
}

