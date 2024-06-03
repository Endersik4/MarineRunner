// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Obstacles/Destructible/DestructibleMeshActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

#include "SpawnDestructibleActor.h"

ADestructibleMeshActor::ADestructibleMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructableMeshComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Destructable Mesh Component"));
	RootComponent = DestructableMeshComponent;
	//ECC_GameTraceChannel4 = WallrunTrace
	DestructableMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
	DestructableMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

	DestructableMeshComponent->ObjectType = EObjectStateTypeEnum::Chaos_Object_Sleeping;
	DestructableMeshComponent->SetHiddenInGame(true);

	FGeometryCollectionDamagePropagationData EasilyBrokenMeshDamageData;
	EasilyBrokenMeshDamageData.bEnabled = true;
	EasilyBrokenMeshDamageData.BreakDamagePropagationFactor = 0.f;
	EasilyBrokenMeshDamageData.ShockDamagePropagationFactor = 5.f;
	DestructableMeshComponent->DamagePropagationData = EasilyBrokenMeshDamageData;

	DestructableMeshComponent->SetNotifyBreaks(true);
}

void ADestructibleMeshActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);

	DestructableMeshComponent->OnChaosBreakEvent.AddDynamic(this, &ADestructibleMeshActor::OnChaosBreakEvent);
}

// Called every frame
void ADestructibleMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibleMeshActor::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	if (!bCanPlayBreakSound)
		return;

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BreakSound, BreakEvent.Location);
	bCanPlayBreakSound = false;

	DisableAssignedDestructibleActor();
	
	FTimerHandle BreakSoundHandle;
	GetWorld()->GetTimerManager().SetTimer(BreakSoundHandle, this, &ADestructibleMeshActor::PlayAgainBreakSound, CanPlayBreakSoundTime, false);
}

void ADestructibleMeshActor::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	DisableAssignedDestructibleActor();

	DestructableMeshComponent->ApplyExternalStrain(NewHit.Item, NewHit.ImpactPoint, ExternalStrainRadius, 0, 1.f, ExternalStrainStrength);
	DestructableMeshComponent->ApplyInternalStrain(NewHit.Item, NewHit.ImpactPoint, InternalStrainRadius, 0, 1.f, InternalStrainStrength);

	if (IsValid(BulletActor))
	{
		DestructableMeshComponent->AddRadialImpulse(BulletActor->GetActorLocation(), FMath::Clamp(NewSphereRadius, 100.f, 3000.f), NewImpulseForce * ImpulseForceMultiplier, ERadialImpulseFalloff::RIF_Linear);

		if (IsValid(DestructionHitSound))
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DestructionHitSound, BulletActor->GetActorLocation());
		else
			UE_LOG(LogTemp, Warning, TEXT("Destruction Hit Sound is nullptr in Destructible Mesh Actor!"));

		if (DestructionHitParticle)
		{
			FTransform DestructionHitParticleTransform = FTransform(BulletActor->GetActorRotation(), BulletActor->GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionHitParticle, DestructionHitParticleTransform);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Destruction Hit Sound is nullptr in Destructible Mesh Actor!!"));
	}
}

void ADestructibleMeshActor::DisableAssignedDestructibleActor()
{
	if (!IsValid(AssignedDestructibleActor) || bVisible)
		return;

	AssignedDestructibleActor->DestructibleMeshActorGotHit();
	bVisible = true;
	DestructableMeshComponent->SetHiddenInGame(false);

}

