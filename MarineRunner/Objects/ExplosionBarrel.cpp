// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Objects/ExplosionBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
AExplosionBarrel::AExplosionBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ExplosionBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosion Barrel Mesh"));
	RootComponent = ExplosionBarrelMesh;

	ExplosionBarrelGeometryMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Explosion Barrel Geometry Mesh"));
	ExplosionBarrelGeometryMesh->SetupAttachment(ExplosionBarrelMesh);

	ExplosionBarrelGeometryMesh->SetVisibility(false);
}

void AExplosionBarrel::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (bExploded == true) return;
	bExploded = true;

	TArray<FHitResult> HitArray;
	GetWorld()->SweepMultiByChannel(HitArray, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(ExplosionRadius));

	if (bDrawDebugRadialSphere) DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 50, FColor::Red, true);

	//Use UseInterfaceOnActor(HitResult) only once on the same actor
	TArray<AActor*> HitActors;
	for (int i = 0; i != HitArray.Num(); i++) { HitActors.AddUnique(HitArray[i].GetActor()); }

	//Use interface on every actors that was hit by SweepMultiByChannel
	for (const FHitResult& HitResult : HitArray)
	{
		if (HitActors.Find(HitResult.GetActor()) >= 0)
		{
			UseInterfaceOnActor(HitResult);
			HitActors.Remove(HitResult.GetActor());
		}
	}

	SpawnEffects();
}

// Called when the game starts or when spawned
void AExplosionBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosionBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosionBarrel::UseInterfaceOnActor(const FHitResult& HitResult)
{
	IInteractInterface* Interface = Cast<IInteractInterface>(HitResult.GetActor());
	if (Interface) //Check if Object has Interface C++ Implementation
	{
		float RadialDamage = ExplosionDamage / (FVector::Distance(GetActorLocation(), HitResult.GetActor()->GetActorLocation()) / 100);
		Interface->ApplyDamage(RadialDamage, ExplosionImpulseForce, HitResult, this, ExplosionRadius);
	}
	else if (HitResult.GetActor()->Implements<UInteractInterface>())  //Check if Object has Interface Blueprint Implementation
	{
		IInteractInterface::Execute_BreakObject(HitResult.GetActor(), ExplosionImpulseForce, HitResult, this, ExplosionRadius);
	}
	else
	{
		if (HitResult.GetComponent()->IsSimulatingPhysics() == true)
		{
			HitResult.GetComponent()->AddRadialImpulse(GetActorLocation(), ExplosionRadius, ExplosionImpulseForce * 10.f, ERadialImpulseFalloff::RIF_Linear);
		}
	}
}

void AExplosionBarrel::SpawnEffects()
{
	if (ExplosionSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), FRotator(0.f), FVector(ExplosionParticleSize));
	}

	float CameraShakeScale = 1 - (FVector::Distance(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation(), GetActorLocation()) / 10000);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->StartCameraShake(CameraShakeAfterExplosion, CameraShakeScale);

	UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ExplosionDecal, FVector(ExplosionDecalSize), GetActorLocation(), FRotator(0.f));
	if (SpawnedDecal)
	{
		SpawnedDecal->SetFadeScreenSize(0.f);
	}

	ExplosionBarrelGeometryMesh->SetVisibility(true);
	ExplosionBarrelMesh->SetVisibility(false);
}

