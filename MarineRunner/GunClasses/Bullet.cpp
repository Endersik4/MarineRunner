// Copyright Adam Bartela.All Rights Reserved

#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "MarineRunner/Interfaces/InteractInterface.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;

	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BulletMesh->bReturnMaterialOnMove = true;

}	

void ABullet::BeginPlay()																	
{
	Super::BeginPlay();

	OnActorHit.AddUniqueDynamic(this, &ABullet::OnHit);

	SetBulletMovementType();

	if (IsValid(BulletTrailNiagaraParticle))
		SpawnedBulletTrailNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailNiagaraParticle, GetActorLocation());
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementBullet(DeltaTime);

	if (IsValid(SpawnedBulletTrailNiagara)) // Changing the location of Bullet Trail in tick instead of attach is for better particle effect.
	{
		SpawnedBulletTrailNiagara->SetWorldLocation(GetActorLocation());
	}
}

#pragma region ///////////////////////////// BULLET MOVEMENT//////////////////////////////////
void ABullet::MovementBullet(float Delta)
{
	if (!bStartBulletMovement)
		return;

	BulletLocation = GetActorLocation() + (GetActorForwardVector() * BulletData.Speed * Delta);
	
	// if the bullet exceed BulletDistance, it will start falling.
	if (TrackBulletDistance >= BulletData.Distance)
	{
		BulletLocation -= GetActorUpVector() * (BulletData.FallingDown * Delta);
		BulletData.FallingDown += BulletFallingDown * Delta;
	}
	else
		TrackBulletDistance += (BulletData.Speed * Delta);

	SetActorLocation(BulletLocation, true, (FHitResult*)nullptr,  ETeleportType::TeleportPhysics);
}

void ABullet::SetBulletMovementType()
{
	if (BulletData.bUsePhysicsForMovement)
	{
		BulletMesh->SetSimulatePhysics(true);
		BulletMesh->SetAllUseCCD(true); 
		BulletMesh->AddImpulse(GetActorForwardVector() * BulletData.Speed);
	}
	else 
		bStartBulletMovement = true;
}
#pragma endregion

#pragma region /////////////////// HIT //////////////////////
bool ABullet::BulletStuckInActor(const FHitResult& Hit)
{
	if (!BulletData.bCanBulletGoThroughObjects) 
		return false;

	if (Hit.GetActor() == HitActor) //If bullet is stuck in the same actor then teleport it a bit forward
	{
		FVector NewBulletLocation = GetActorLocation() + GetActorForwardVector() * BulletStuckInActorTeleportValue;
		SetActorLocation(NewBulletLocation);
		return true;
	}
	HitActor = Hit.GetActor();
	return false;
}

void ABullet::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BulletStuckInActor(Hit)) 
		return;

	if (BulletData.bShouldCameraShakeAfterHit)
	{
		float DistanceToPlayer = FVector::Distance(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation(), GetActorLocation());
		if (DistanceToPlayer < MaxDistanceToStartShake && DistanceToPlayer != 0)
		{
			float CameraShakeScale = (MaxDistanceToStartShake / DistanceToPlayer) * CameraShakeScaleMultiplier;
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->StartCameraShake(BulletData.CameraShakeAfterBulletHit, CameraShakeScale);
		}
	}

	if (BulletData.bUseSphereForDamage)
		SphereRadialDamage(Hit);
	else 
		UseDamageInterfaceOnActor(Hit);

	BulletThroughObject(Hit);
}

void ABullet::SphereRadialDamage(const FHitResult& Hit)
{
	TArray<FHitResult> HitArray;
	GetWorld()->SweepMultiByChannel(HitArray, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(BulletData.RadialSphereRadius));

#ifdef WITH_EDITOR
	if (BulletData.bDrawDebugRadialSphere) DrawDebugSphere(GetWorld(), GetActorLocation(), BulletData.RadialSphereRadius, 50, FColor::Red, true);
#endif // !WITH_EDITOR

	//Use UseDamageInterfaceOnActor(HitResult) only once on the same actor
	TArray<TObjectPtr<AActor>> ActorsToApplyDamage;
	for (int i = 0; i != HitArray.Num(); i++) 
	{ 
		ActorsToApplyDamage.AddUnique(HitArray[i].GetActor()); 
	}

	//Use interface on every actors that was hit by SweepMultiByChannel
	for (const FHitResult& HitResult : HitArray)
	{
		if (ActorsToApplyDamage.Find(HitResult.GetActor()) == INDEX_NONE)
			continue;

		UseDamageInterfaceOnActor(HitResult);
		ActorsToApplyDamage.Remove(HitResult.GetActor());
	}

	SpawnEffectsWhenHit(Hit);
	Destroy();
}

void ABullet::UseDamageInterfaceOnActor(const FHitResult& HitResult)
{
	IInteractInterface* DamageInterface = Cast<IInteractInterface>(HitResult.GetActor());
	if (DamageInterface) // C++ Interface function
	{
		DamageInterface->ApplyDamage(BulletData.Damage, BulletData.HitImpulseForce, HitResult, this);
	}
	else if (HitResult.GetActor()->Implements<UInteractInterface>()) // Blueprint Interface function
	{
		IInteractInterface::Execute_BreakObject(HitResult.GetActor(), BulletData.HitImpulseForce, HitResult, this, BulletData.RadialSphereRadius);
	}
	else
		HitActorWithoutInterface(HitResult);
}

void ABullet::HitActorWithoutInterface(const FHitResult& HitResult)
{
	if (BulletData.bUseSphereForDamage)
	{
		if (HitResult.GetComponent()->IsSimulatingPhysics())
		{
			HitResult.GetComponent()->AddRadialImpulse(GetActorLocation(), BulletData.RadialSphereRadius, BulletData.HitImpulseForce * BulletImpulseMultiplier, ERadialImpulseFalloff::RIF_Linear);
		}
		return;
	}

	if (HitResult.GetComponent()->IsSimulatingPhysics())
	{
		FVector Impulse = GetActorForwardVector() * BulletData.HitImpulseForce * BulletImpulseMultiplier;
		HitResult.GetComponent()->AddImpulse(Impulse);
	}

	SpawnEffectsWhenHit(HitResult);
}

void ABullet::BulletThroughObject(const FHitResult& Hit)
{
	if (!BulletData.bCanBulletGoThroughObjects 
		|| BulletData.MaxObjectsForBulletToGoThrough <= 0 || UGameplayStatics::GetSurfaceType(Hit) != EPhysicalSurface::SurfaceType2)
	{
		Destroy();
		return;
	}

	BulletData.Damage *= BulletData.DamageReduceAfterObject;
	BulletData.HitImpulseForce *= BulletData.ImpulseReduceAfterObject;
	BulletData.MaxObjectsForBulletToGoThrough--;

	FVector MoveLocation = GetActorLocation() + GetActorForwardVector() * BulletStuckInActorTeleportValue;
	SetActorLocation(MoveLocation);
}
#pragma endregion

#pragma region ////////////////////// EFFECTS ////////////////////////////
void ABullet::SpawnEffectsWhenHit(const FHitResult& Hit)
{
	if (IsValid(BulletHitSound))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BulletHitSound, Hit.ImpactPoint);
	if (BulletHitParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticle, Hit.ImpactPoint, GetActorRotation(), BulletHitParticleSize);

	SpawnBulletHoleDecal(Hit);
}

void ABullet::SpawnBulletHoleDecal(const FHitResult& Hit)
{
	if (!IsValid(BulletHoleDecalMaterial)) 
		return;

	TObjectPtr<UDecalComponent> SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalMaterial, BulletHoleDecalSize, Hit.Location, Hit.ImpactNormal.Rotation());
	if (IsValid(SpawnedDecal))
	{
		SpawnedDecal->SetFadeScreenSize(0.f);
		SpawnedDecal->DecalSize.X += BulletHoleSize_X;
		SpawnedDecal->SetFadeOut(BulletHoleFadeOutStartDelay, BulletHoleFadeOutDuration);
	}
}
#pragma endregion