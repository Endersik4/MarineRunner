// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Enemies/Components/EnemyGunComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Gun/Bullet/Bullet.h"
#include "MarineRunner/Enemies/Interfaces/EnemyGunInterface.h"

UEnemyGunComponent::UEnemyGunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyGunComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GetOwner()))
		OwningEnemyInterface = Cast<IEnemyInterface>(GetOwner());

	InitialMagazineCapacity = MagazineCapacity;
}

void UEnemyGunComponent::Shoot()
{
	if (!CanShoot())
		return;

	ShootEffects();

	SpawnManyBullets();

	MagazineCapacity--;

	AddImpulseDuringShooting();

	bCanShoot = false;

	if (MagazineCapacity == 0)
		Reload();
}

bool UEnemyGunComponent::CanShoot()
{
	if (!IsValid(BulletData.BulletClass))
		return false;

	if (!bCanShoot)
		return false;

	return true;
}

void UEnemyGunComponent::ShootEffects()
{
	if (!IsValid(OwningEnemyInterface->GetSkeletalMesh()))
		return;

	if (IsValid(ShootingSound))
		UGameplayStatics::SpawnSoundAttached(ShootingSound, OwningEnemyInterface->GetSkeletalMesh(), MuzzleFleshSocketNames[CurrentSocketNameIndex]);
	else
		UE_LOG(LogTemp, Warning, TEXT("Shooting Sound is nullptr in EnemyGunComponent!"));

	if (ShootParticle)
		UParticleSystemComponent* SpawnedShootParticle = UGameplayStatics::SpawnEmitterAttached(ShootParticle, OwningEnemyInterface->GetSkeletalMesh(), MuzzleFleshSocketNames[CurrentSocketNameIndex], FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	else
		UE_LOG(LogTemp, Warning, TEXT("Spawned Shoot Particle is nullptr in EnemyGunComponent!"));
}

void UEnemyGunComponent::SpawnManyBullets()
{
	if (bManyBulletAtOnce)
	{
		for (int i = 0; i != HowManyBulletsToSpawn; i++)
		{
			SpawnBullet();
		}
	}
	else
	{
		SpawnBullet();
	}
}

void UEnemyGunComponent::SpawnBullet()
{
	if (!OwningEnemyInterface)
		return;
	if (!IsValid(OwningEnemyInterface->GetSkeletalMesh()))
		return;

	const FRotator& BulletRotation = CalculateBulletRotation();

	const FTransform& BulletTransform = FTransform(BulletRotation, OwningEnemyInterface->GetSkeletalMesh()->GetSocketLocation(BulletSocketsNames[CurrentSocketNameIndex]));
	TObjectPtr<ABullet> SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletData.BulletClass, BulletTransform);
	if (!IsValid(SpawnedBullet))
		return;

	FBulletStruct BulletDataForSpawnedBullet = BulletData;
	BulletDataForSpawnedBullet.Damage = (bManyBulletAtOnce == false ? BulletData.Damage : BulletData.Damage / HowManyBulletsToSpawn);
	BulletDataForSpawnedBullet.HitImpulseForce = (bManyBulletAtOnce == false ? BulletData.HitImpulseForce : BulletData.HitImpulseForce / HowManyBulletsToSpawn);
	if (BulletData.bUsePhysicsForMovement)
	{
		const FVector& Gravity = FVector(0.f, 0.f, -980.f);
		const float& CalculatedImpulse_X = FMath::Abs((OwningEnemyInterface->GetFocusedActor()->GetActorLocation() - BulletTransform.GetLocation() - (Gravity / 2)).X);
		BulletDataForSpawnedBullet.Speed = BulletData.Speed * CalculatedImpulse_X;
	}

	SpawnedBullet->SetBulletData(BulletDataForSpawnedBullet);
	SpawnedBullet->FinishSpawning(BulletTransform);

	CurrentSocketNameIndex++;
	if (CurrentSocketNameIndex >= BulletSocketsNames.Num())
	{
		CurrentSocketNameIndex = 0;
	}
}

const FVector UEnemyGunComponent::PredictWhereToShoot(const bool bIgnoreOffset)
{
	const TObjectPtr<AActor> CurrentFocusedActor = OwningEnemyInterface->GetFocusedActor();
	if (!IsValid(CurrentFocusedActor))
		return FVector(0.f);

	FVector FocusedActorLocation = CurrentFocusedActor->GetActorLocation();

	if (bIgnoreOffset)
		return FocusedActorLocation;

	FocusedActorLocation += CurrentFocusedActor->GetActorUpVector() * PredictWhereToShootOffset_UP;
	FocusedActorLocation += CurrentFocusedActor->GetActorRightVector() * CurrentFocusedActor->GetInputAxisValue("Right") * PredictWhereToShootOffset_Right ;
	FocusedActorLocation += CurrentFocusedActor->GetRootComponent()->GetForwardVector() * CurrentFocusedActor->GetInputAxisValue("Forward") * PredictWhereToShootOffset_Forward;
	return FocusedActorLocation;
}

const FRotator UEnemyGunComponent::CalculateBulletRotation()
{
	FRotator BulletRotation;
	if (bFindBulletRotationTowardsTarget)
	{
		BulletRotation = UKismetMathLibrary::FindLookAtRotation(OwningEnemyInterface->GetSkeletalMesh()->GetSocketLocation(BulletSocketsNames[CurrentSocketNameIndex]), PredictWhereToShoot());
	}
	else
	{
		BulletRotation = OwningEnemyInterface->GetSkeletalMesh()->GetSocketTransform(BulletSocketsNames[CurrentSocketNameIndex]).Rotator();
	}

	//Bullet will randomly "go" to other directions 
	BulletRotation.Pitch += FMath::FRandRange(RandomRangeForPitchBullet.GetLowerBoundValue(), RandomRangeForPitchBullet.GetUpperBoundValue());
	BulletRotation.Yaw += FMath::FRandRange(RandomRangeForYawBullet.GetLowerBoundValue(), RandomRangeForYawBullet.GetUpperBoundValue());

	return BulletRotation;
}

void UEnemyGunComponent::AddImpulseDuringShooting()
{
	const FVector& RecoilImpulse = -GetOwner()->GetActorForwardVector() * RecoilImpulseOnEnemy;
	OwningEnemyInterface->AddImpulseToPhysicsMesh(RecoilImpulse);
}

void UEnemyGunComponent::Reload()
{
	if (bIsReloading)
		return;

	bIsReloading = true;
	bCanShoot = false;
	GetWorld()->GetTimerManager().SetTimer(DelayEmptyMagazineHandle, this, &UEnemyGunComponent::DelayAfterEmptyMagazine, DelayTimeMagazine, false);
}

void UEnemyGunComponent::DelayAfterEmptyMagazine()
{
	MagazineCapacity = InitialMagazineCapacity;
	bCanShoot = true;
	bIsReloading = false;
}

const bool UEnemyGunComponent::CanShootAgain()
{
	bCanShoot = bIsReloading == false && MagazineCapacity > 0;
	return bCanShoot;
}
