// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/EnemySpawner.h"
#include "Components/BoxComponent.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"

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
	
	SpawnEnemiesBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnSpawnEnemiesBoxBeginOverlap);
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

			GetWorld()->SpawnActor<AEnemyPawn>(CurrentEnemy.EnemyClassToSpawn, CurrentEnemyTransform);
		}
	}
}

