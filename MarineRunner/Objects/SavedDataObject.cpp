// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/SavedDataObject.h"
#include "MarineRunner/Objects/Door/Door.h"
#include "MarineRunner/Objects/ChestWithItems.h"

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

void ASavedDataObject::LoadObjectsData()
{
	for (ADoor* CurrentSavedDoor : SavedDoor)
	{
		if (IsValid(CurrentSavedDoor) == false)
			continue;

		CurrentSavedDoor->ChangeToUsePin(false);
	}
}
