// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Obstacles/Destructible/SpawnDestructibleActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"
#include "DestructibleMeshActor.h"

ASpawnDestructibleActor::ASpawnDestructibleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MainMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh Component"));
	RootComponent = MainMeshComponent;
	MainMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MainMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MainMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void ASpawnDestructibleActor::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(SpawnDestructibleMeshHandle, this, &ASpawnDestructibleActor::SpawnDestructibleMesh, SpawnDestructibleMeshDelay, false);
}

void ASpawnDestructibleActor::SpawnDestructibleMesh()
{
	if (bSaved)
		return;

	if (!IsValid(DestructibleMeshActorClass))
		return;

	SpawnedDestructibleMeshActor = GetWorld()->SpawnActor<ADestructibleMeshActor>(DestructibleMeshActorClass, GetActorLocation(), GetActorRotation());
	if (!IsValid(SpawnedDestructibleMeshActor))
		return;

	SpawnedDestructibleMeshActor->SetAssignedDestructibleActor(this);
}

void ASpawnDestructibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnDestructibleActor::DestructibleMeshActorGotHit()
{
	if (bSaved)
		return;

	MainMeshComponent->SetHiddenInGame(true, true);
	MainMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bSaved = true;

	AddActorToSavedDataObject();
}

void ASpawnDestructibleActor::AddActorToSavedDataObject()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();
	
	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

#pragma region ////////////// SaveCustomDataInterface Functions //////////
void ASpawnDestructibleActor::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (SavedCustomData.ObjectState == 1)
	{
		MainMeshComponent->SetHiddenInGame(true, true);
		MainMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bSaved = true;
	}
}

void ASpawnDestructibleActor::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{

}

void ASpawnDestructibleActor::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	MainMeshComponent->SetHiddenInGame(false, true);
	MainMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	bSaved = false;

	if (IsValid(SpawnedDestructibleMeshActor))
		SpawnedDestructibleMeshActor->Destroy();

	SpawnDestructibleMesh();
}
#pragma endregion
