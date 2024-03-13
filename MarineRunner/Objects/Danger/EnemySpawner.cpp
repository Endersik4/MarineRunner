// Copyright Adam Bartela.All Rights Reserved


#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Enemies/TypesOfEnemy/Base/EnemyBasePawn.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnEnemiesBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnEnemiesBox"));
	RootComponent = SpawnEnemiesBox;

	SpawnEnemiesBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpawnEnemiesBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(ActivateEnemySpawnerHandle, this, &AEnemySpawner::EnableSpawnEnemiesBoxBeginOverlap, DelayToActivateEnemySpawner, false);
}

void AEnemySpawner::OnSpawnEnemiesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bEnemiesSpawned)
		return;

	SpawnAllEnemiesFromSpawner();

	bEnemiesSpawned = true;
}

void AEnemySpawner::SpawnAllEnemiesFromSpawner()
{
	for (const FEnemiesSpawner& CurrentEnemy : EnemiesSpawner)
	{
		for (const FTransform& CurrentEnemyTransform : CurrentEnemy.EnemiesTransform)
		{
			if (!IsValid(CurrentEnemy.EnemyClassToSpawn))
				continue;

			TObjectPtr<AEnemyPawn> SpawnedEnemy = GetWorld()->SpawnActor<AEnemyPawn>(CurrentEnemy.EnemyClassToSpawn, CurrentEnemyTransform);
			if (!IsValid(SpawnedEnemy))
				continue;

			SpawnedEnemy->SaveEnemySpawnedDataAtRuntime();
		}
	}

	EnemiesSpawnedSaveData();
}

void AEnemySpawner::EnemiesSpawnedSaveData()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));
	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0) 
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void AEnemySpawner::DisableEnemySpawner(bool bDisable)
{
	SetActorTickEnabled(!bDisable);
	SetActorHiddenInGame(bDisable);
	SetActorEnableCollision(!bDisable);
	bEnemiesSpawned = bDisable;
}

void AEnemySpawner::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1)
		DisableEnemySpawner();
}

void AEnemySpawner::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AEnemySpawner::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	DisableEnemySpawner(false);
}

void AEnemySpawner::EnableSpawnEnemiesBoxBeginOverlap()
{
	if (bEnemiesSpawned)
		return;

	SpawnEnemiesBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemySpawner::OnSpawnEnemiesBoxBeginOverlap);
}
