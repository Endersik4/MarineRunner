// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/EnemiesClasses/EnemyAiController.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Interfaces/InteractInterface.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;

	//BulletMesh->SetMassScale(NAME_None, 1.5f);
	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BulletMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); //For the enemy bullet set this to BLOCK
	BulletMesh->bReturnMaterialOnMove = true;

	OnActorHit.AddDynamic(this, &ABullet::OnHit);
	Tags.Add(TEXT("Bullet"));
}	

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementBullet(DeltaTime);
}

void ABullet::MovementBullet(float Delta)
{
	if (bUseMyMovement == false) return;

	BulletLocation = GetActorLocation() + (GetActorForwardVector() * AmmoSpeed * Delta);
	
	if (TrackBulletDistance >= AmmoDistance)
	{
		BulletLocation -= GetActorUpVector() * (AmmoFallingDown * Delta);
		AmmoFallingDown += 20 * Delta;
	}
	else
	{
		TrackBulletDistance += (AmmoSpeed * Delta);
	}

	SetActorLocation(BulletLocation, true);
}

void ABullet::ImpulseOnBullet()
{
	if (bUseImpulseForBullet)
	{
		BulletMesh->SetSimulatePhysics(true);
		FVector Impulse = GetActorForwardVector() * AmmoSpeed;
		BulletMesh->AddImpulse(Impulse);
	}
	else bUseMyMovement = true;
}

void ABullet::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Bullet")) return;

	IInteractInterface* Interface = Cast<IInteractInterface>(Hit.GetActor());
	if (Interface) //Check if Object has Interface C++ Implementation
	{
		Interface->ApplyDamage(Damage, AmmoImpulseForce, GetActorForwardVector(), Hit);
	}
	else if (OtherActor->Implements<UInteractInterface>())  //Check if Object has Interface Blueprint Implementation
	{
		IInteractInterface::Execute_BreakObject(Hit.GetActor(),  AmmoImpulseForce, GetActorRotation(), Hit); 
	}
	else {	SpawnEffectsForImpact(Hit); }

	BulletThroughObject(Hit);
}


void ABullet::SpawnEffectsForImpact(const FHitResult& Hit)
{
	if (ObjectHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ObjectHitSound, Hit.ImpactPoint);
	if (BulletHitParticle)
	{
		FRotator Rotation = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticle, Hit.ImpactPoint, Rotation);
		if (BulletHit2Particle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHit2Particle, Hit.ImpactPoint, Rotation);
		}
	}

	SpawnBulletHole(Hit);
}

void ABullet::SpawnBulletHole(const FHitResult& Hit)
{
	if (!BulletHoleDecalMaterial) return;

	FVector Size = FVector(FMath::FRandRange(7.f, 8.f));
	FRotator Rotation = Hit.ImpactNormal.Rotation();
	UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalMaterial, Size, Hit.Location, Rotation);
	if (SpawnedDecal)
	{
		SpawnedDecal->SetFadeScreenSize(0.f);
		SpawnedDecal->SetLifeSpan(10.f);
	}
}

void ABullet::BulletThroughObject(const FHitResult& Hit)
{
	if (bCanBulletGoThrough == false || MaxObjectsForBulletToGoThrough <= 0 || UGameplayStatics::GetSurfaceType(Hit) != EPhysicalSurface::SurfaceType2)
	{
		SetActorTickEnabled(false);
		Destroy();
		return;
	}

	Damage *= DamageReduceAfterObject;
	AmmoImpulseForce *= ImpulseReduceAfterObject;
	MaxObjectsForBulletToGoThrough--;

	FVector MoveLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
	SetActorLocation(MoveLocation);
}

void ABullet::SetBulletVariables(float NewDamage, float NewAmmoSpeed, float NewAmmoDistance, float NewAmmoFallingDown, float NewAmmoImpulseForce)
{
	Damage = NewDamage;
	AmmoSpeed = NewAmmoSpeed;
	AmmoDistance = NewAmmoDistance;
	AmmoFallingDown = NewAmmoFallingDown;
	AmmoImpulseForce = NewAmmoImpulseForce;
}

void ABullet::SetBulletGoThroughVariables(bool NewCanBulletGoThrough, float NewDamageReduceAfterObject, float NewImpulseReduceAfterObject, int32 NewMaxObjectsForBulletToGoThrough)
{
	if (NewCanBulletGoThrough == false) return;

	bCanBulletGoThrough = true;
	DamageReduceAfterObject = NewDamageReduceAfterObject;
	ImpulseReduceAfterObject = NewImpulseReduceAfterObject;
	MaxObjectsForBulletToGoThrough = NewMaxObjectsForBulletToGoThrough;
}