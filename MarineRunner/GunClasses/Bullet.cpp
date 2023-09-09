// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

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

#pragma region ///////////////////////////// BULLET MOVEMENT//////////////////////////////////
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

	SetActorLocation(BulletLocation, true, (FHitResult*)nullptr,  ETeleportType::TeleportPhysics);
}

void ABullet::SetUpBullet(bool bShouldUseImpulseOnBullet)
{
	if (bShouldUseImpulseOnBullet)
	{
		BulletMesh->SetSimulatePhysics(true);
		BulletMesh->SetAllUseCCD(true); 
		BulletMesh->AddImpulse(GetActorForwardVector() * AmmoSpeed);
	}
	else bUseMyMovement = true;
}
#pragma endregion

#pragma region /////////////////// HIT //////////////////////
bool ABullet::BulletStuckInActor(const FHitResult& Hit)
{
	if (Hit.GetActor() == HitActor) //If bullet is stuck in the same actor then teleport it a bit forward
	{
		FVector NewLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
		SetActorLocation(NewLocation);
		return true;
	}
	HitActor = Hit.GetActor();
	return false;
}

void ABullet::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BulletStuckInActor(Hit) == true) return;

	if (bShouldCameraShakeAfterHit)
	{
		float CameraShakeScale = 1 - (FVector::Distance(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation(), GetActorLocation()) / 10000);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->StartCameraShake(CameraShakeAfterHit, CameraShakeScale);
	}

	SphereRadialDamage(Hit);
	
	UseInterfaceOnActor(Hit);
	
	BulletThroughObject(Hit);
}

void ABullet::SphereRadialDamage(const FHitResult& Hit)
{
	if (bSphereImpulse == false) return;

	TArray<FHitResult> HitArray;
	GetWorld()->SweepMultiByChannel(HitArray, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(SphereImpulseRadius));

	if (bDrawDebugRadialSphere) DrawDebugSphere(GetWorld(), GetActorLocation(), SphereImpulseRadius, 50, FColor::Red, true);

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

	SpawnEffectsWhenHit(Hit);
	Destroy();
}

void ABullet::UseInterfaceOnActor(const FHitResult& HitResult)
{
	IInteractInterface* Interface = Cast<IInteractInterface>(HitResult.GetActor());
	if (Interface) //Check if Object has Interface C++ Implementation
	{
		if (SphereImpulseRadius != 0.f)
		{
			float RadialDamage = Damage / (FVector::Distance(GetActorLocation(), HitResult.GetActor()->GetActorLocation()) / 100);
			Interface->ApplyDamage(RadialDamage, AmmoImpulseForce, HitResult, this, SphereImpulseRadius);
		}
		else Interface->ApplyDamage(Damage, AmmoImpulseForce, HitResult, this);
	}
	else if (HitResult.GetActor()->Implements<UInteractInterface>())  //Check if Object has Interface Blueprint Implementation
	{
		IInteractInterface::Execute_BreakObject(HitResult.GetActor(), AmmoImpulseForce, HitResult, this, SphereImpulseRadius);
	}
	else
	{
		HitActorWithoutInterface(HitResult);
	}
}

void ABullet::HitActorWithoutInterface(const FHitResult& HitResult)
{
	if (HitResult.GetComponent()->IsSimulatingPhysics() == true)
	{
		if (SphereImpulseRadius != 0.f)
		{
			HitResult.GetComponent()->AddRadialImpulse(GetActorLocation(), SphereImpulseRadius, AmmoImpulseForce * 10.f, ERadialImpulseFalloff::RIF_Linear);
		}
		else
		{
			FVector Impulse = GetActorForwardVector() * AmmoImpulseForce * 10.f;
			HitResult.GetComponent()->AddImpulse(Impulse);
		}
	}
	if (SphereImpulseRadius == 0.f) SpawnEffectsWhenHit(HitResult);
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
#pragma endregion

#pragma region ////////////////////// EFFECTS ////////////////////////////
void ABullet::SpawnEffectsWhenHit(const FHitResult& Hit)
{
	if (ObjectHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ObjectHitSound, Hit.ImpactPoint);
	if (BulletHitParticle)
	{
		FRotator Rotation = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticle, Hit.ImpactPoint, Rotation, BulletHitParticleSize);
		if (BulletHit2Particle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHit2Particle, Hit.ImpactPoint, Rotation);
		}
	}

	SpawnBulletHoleDecal(Hit);
}

void ABullet::SpawnBulletHoleDecal(const FHitResult& Hit)
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
#pragma endregion

#pragma region ///////////////////// SETTING THE BULLET /////////////////////
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

void ABullet::RadialImpulse(float SphereRadius, bool bShouldDrawDebugSphere)
{
	bSphereImpulse = true;
	SphereImpulseRadius = SphereRadius;
	bDrawDebugRadialSphere = bShouldDrawDebugSphere;
}

void ABullet::SetCameraShake(TSubclassOf<UCameraShakeBase> NewCameraShake)
{
	bShouldCameraShakeAfterHit = true;
	CameraShakeAfterHit = NewCameraShake;
}
#pragma endregion