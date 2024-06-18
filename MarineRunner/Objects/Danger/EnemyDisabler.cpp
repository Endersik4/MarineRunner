// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Danger/EnemyDisabler.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Enemies/TypesOfEnemy/Base/EnemyBasePawn.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

AEnemyDisabler::AEnemyDisabler()
{
	PrimaryActorTick.bCanEverTick = false;

	DisableEnemiesBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Disable Enemies Box"));
	RootComponent = DisableEnemiesBox;

	DisableEnemiesBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DisableEnemiesBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AEnemyDisabler::BeginPlay()
{
	Super::BeginPlay();

	DisableEnemiesBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyDisabler::OnDisableEnemiesBoxBeginOverlap);
}

void AEnemyDisabler::OnDisableEnemiesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DestroyAllEnemiesOnMap();

	ResetDetectedEnemies();
}

void AEnemyDisabler::DestroyAllEnemiesOnMap()
{
	TArray<TObjectPtr<AActor>> AllEnemiesOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass(), AllEnemiesOnMap);

	for (TObjectPtr<AActor> CurrentEnemy : AllEnemiesOnMap)
	{
		if (!IsValid(CurrentEnemy))
			continue;

		CurrentEnemy->Destroy();
	}
}

void AEnemyDisabler::ResetDetectedEnemies()
{
	TObjectPtr<UGameInstance> TempGameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!IsValid(TempGameInstance))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(TempGameInstance);
	if (!IsValid(GameInstance))
		return;

	GameInstance->ResetDetectedEnemy();
}

