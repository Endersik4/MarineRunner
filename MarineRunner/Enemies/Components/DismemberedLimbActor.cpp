// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/Components/DismemberedLimbActor.h"
#include "Components/SkeletalMeshComponent.h"

#include "DismemberEnemyComponent.h"

ADismemberedLimbActor::ADismemberedLimbActor()
{
	PrimaryActorTick.bCanEverTick = false;

	LimbSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Limb Skeletal Mesh"));
	RootComponent = LimbSkeletalMesh;

	LimbSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LimbSkeletalMesh->SetCollisionProfileName(FName("PhysicsActor"));
	LimbSkeletalMesh->SetSimulatePhysics(true);
	LimbSkeletalMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Ignore); // trace == in air


	DismemberBodyComponent = CreateDefaultSubobject<UDismemberEnemyComponent>(TEXT("Dismember Body Component"));
}

void ADismemberedLimbActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADismemberedLimbActor::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	DismemberBodyComponent->DismemberLimb(LimbSkeletalMesh, NewHit, NewImpulseForce, FColor::Purple, NewSphereRadius);
}

