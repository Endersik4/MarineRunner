// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/Objects/SavedDataObject.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::OnSpawnEnemiesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bEnemiesSpawned == true)
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
			if (CurrentEnemy.EnemyClassToSpawn == nullptr)
				continue;

			AEnemyPawn* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyPawn>(CurrentEnemy.EnemyClassToSpawn, CurrentEnemyTransform);
			if (IsValid(SpawnedEnemy) == false)
				continue;

			SpawnedEnemy->SaveEnemySpawnedDataAtRuntime();
		}
	}

	EnemiesSpawnedSaveData();
}

void AEnemySpawner::EnemiesSpawnedSaveData()
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
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
	if (bEnemiesSpawned == true)
		return;

	SpawnEnemiesBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnSpawnEnemiesBoxBeginOverlap);
}
