// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/Components/DismemberedLimbActor.h"
#include "Components/SkeletalMeshComponent.h"

#include "DismemberEnemyComponent.h"
#include "MarineRunner/Enemies/TypesOfEnemy/Base/EnemyBasePawn.h"

ADismemberedLimbActor::ADismemberedLimbActor()
{
	PrimaryActorTick.bCanEverTick = false;

	LimbSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Limb Skeletal Mesh"));
	RootComponent = LimbSkeletalMesh;

	LimbSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LimbSkeletalMesh->SetCollisionProfileName(FName("PhysicsActor"));
	LimbSkeletalMesh->SetSimulatePhysics(true);
	LimbSkeletalMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Ignore); // trace == in air
}

void ADismemberedLimbActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADismemberedLimbActor::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (!IsValid(EnemyLimbOwner))
		return;

	EnemyLimbOwner->SpawnGunshotWoundDecal(NewHit, LimbSkeletalMesh);
	EnemyLimbOwner->SpawnEffectsForImpact(NewHit, EnemyLimbOwner->GetHitBoneType(NewHit.BoneName));
	EnemyLimbOwner->SpawnBloodOnObjectDecal(BulletActor, NewHit.Location);
	EnemyLimbOwner->GetDismemberEnemyComponent()->DismemberLimb(LimbSkeletalMesh, NewHit, NewImpulseForce, FColor::Purple, NewSphereRadius);

	// If the radius of the sphere is 0, the bullet did not use the radial sphere to deal damage.
	if (NewSphereRadius == 0.f && IsValid(BulletActor))
	{
		LimbSkeletalMesh->AddImpulse(BulletActor->GetActorForwardVector() * NewImpulseForce, NewHit.BoneName);
	}
	else
	{
		LimbSkeletalMesh->AddRadialImpulse(NewHit.TraceStart, NewSphereRadius, NewImpulseForce, ERadialImpulseFalloff::RIF_Linear);
	}
}

void ADismemberedLimbActor::SetEnemyLimbOwner(TObjectPtr<AActor> NewOwner)
{
	if (!IsValid(NewOwner))
		return;

	EnemyLimbOwner = Cast<AEnemyPawn>(NewOwner);
}
