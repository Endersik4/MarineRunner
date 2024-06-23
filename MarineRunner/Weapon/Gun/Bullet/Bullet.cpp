// Copyright Adam Bartela.All Rights Reserved

#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "MarineRunner/Interfaces/DamageInterface.h"

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

	SetLifeSpan(BulletLifeSpan);

	SetBulletMovementType();

	if (IsValid(BulletTrailNiagaraParticle))
		SpawnedBulletTrailNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailNiagaraParticle, GetActorLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("Bullet Trail Niagara Particle is nullptr in Bullet!"));
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
	{
		TrackBulletDistance += (BulletData.Speed * Delta);
	}

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
	{
		bStartBulletMovement = true;
	}
}
#pragma endregion

#pragma region /////////////////// HIT //////////////////////
bool ABullet::BulletStuckInActor(const FHitResult& Hit)
{
	if (!BulletData.bCanBulletGoThroughObjects) 
		return false;

	if (Hit.GetActor() == HitActor) //If bullet is stuck in the same actor then teleport it a bit forward
	{
		const FVector NewBulletLocation = GetActorLocation() + GetActorForwardVector() * BulletStuckInActorTeleportValue;
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
		TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (IsValid(Player))
		{
			TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			const float DistanceToPlayer = FVector::Distance(Player->GetActorLocation(), GetActorLocation());
			if (DistanceToPlayer < MaxDistanceToStartShake&& DistanceToPlayer != 0 && IsValid(PlayerController))
			{
				float CameraShakeScale = (MaxDistanceToStartShake / DistanceToPlayer) * CameraShakeScaleMultiplier;
				CameraShakeScale = CameraShakeScale > MaxCameraShakeScale ? MaxCameraShakeScale : CameraShakeScale;
				PlayerController->PlayerCameraManager->StartCameraShake(BulletData.CameraShakeAfterBulletHit, CameraShakeScale);
			}
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

	//Use interface on every actors that was hit by SweepMultiB	yChannel
	for (const FHitResult& HitResult : HitArray)
	{
		UseDamageInterfaceOnActor(HitResult);
	}

	SpawnEffectsWhenHit(Hit);
	if (IsValid(SpawnedBulletTrailNiagara))
		SpawnedBulletTrailNiagara->DestroyComponent();

	Destroy();
}

void ABullet::UseDamageInterfaceOnActor(const FHitResult& HitResult)
{
	IDamageInterface* DamageInterface = Cast<IDamageInterface>(HitResult.GetActor());
	if (DamageInterface) // C++ Interface function
	{
		DamageInterface->ApplyDamage(BulletData.Damage, BulletData.HitImpulseForce, HitResult, this, BulletData.RadialSphereRadius);
	}
	else if (HitResult.GetActor()->Implements<UDamageInterface>()) // Blueprint Interface function
	{
		IDamageInterface::Execute_BreakObject(HitResult.GetActor(), BulletData.HitImpulseForce, HitResult, this, BulletData.RadialSphereRadius);
	}
	else
	{
		HitActorWithoutInterface(HitResult);
	}
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
		const FVector Impulse = GetActorForwardVector() * BulletData.HitImpulseForce * BulletImpulseMultiplier;
		HitResult.GetComponent()->AddImpulse(Impulse);
	}

	SpawnEffectsWhenHit(HitResult);
}

void ABullet::BulletThroughObject(const FHitResult& Hit)
{
	if (!BulletData.bCanBulletGoThroughObjects 
		|| BulletData.MaxObjectsForBulletToGoThrough <= 0 || UGameplayStatics::GetSurfaceType(Hit) != EPhysicalSurface::SurfaceType2)
	{
		if (IsValid(SpawnedBulletTrailNiagara))
			SpawnedBulletTrailNiagara->DestroyComponent();

		Destroy();
		return;
	}

	BulletData.Damage *= BulletData.DamageReduceAfterObject;
	BulletData.HitImpulseForce *= BulletData.ImpulseReduceAfterObject;
	BulletData.MaxObjectsForBulletToGoThrough--;

	const FVector MoveLocation = GetActorLocation() + GetActorForwardVector() * BulletStuckInActorTeleportValue;
	SetActorLocation(MoveLocation);
}
#pragma endregion

#pragma region ////////////////////// EFFECTS ////////////////////////////
void ABullet::SpawnEffectsWhenHit(const FHitResult& Hit)
{
	if (IsValid(BulletHitSound))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BulletHitSound, Hit.ImpactPoint);
	else
		UE_LOG(LogTemp, Warning, TEXT("Bullet Hit Sound is nullptr in Bullet!"));

	if (BulletHitParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticle, Hit.ImpactPoint, GetActorRotation(), BulletHitParticleSize);
	else
		UE_LOG(LogTemp, Warning, TEXT("Bullet Hit Particle is nullptr in Bullet!"));

	SpawnBulletHoleDecal(Hit);
}

void ABullet::SpawnBulletHoleDecal(const FHitResult& Hit)
{
	if (!IsValid(BulletHoleDecalMaterial)) 
		return;

	TObjectPtr<UDecalComponent> SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalMaterial, BulletHoleDecalSize, Hit.Location, Hit.ImpactNormal.Rotation());
	if (!IsValid(SpawnedDecal))
		return;

	SpawnedDecal->SetFadeScreenSize(0.f);
	SpawnedDecal->DecalSize.X += BulletHoleSize_X;
	SpawnedDecal->SetFadeOut(BulletHoleFadeOutStartDelay, BulletHoleFadeOutDuration);
}
#pragma endregion