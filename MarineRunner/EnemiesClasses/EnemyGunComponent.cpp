// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/EnemiesClasses/EnemyGunComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/EnemiesClasses/EnemyInterface.h"

UEnemyGunComponent::UEnemyGunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyGunComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningEnemyInterface = Cast<IEnemyInterface>(GetOwner());
	InitialMagazineCapacity = MagazineCapacity;
}

void UEnemyGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEnemyGunComponent::Shoot()
{
	if (CanShoot() == false)
		return;

	ShootEffects();

	//Had to do this in this way because bullet was spawned before bones were on their locations (in shooting animation)
	GetWorld()->GetTimerManager().SetTimer(ImpulseOnBulletHandle, this, &UEnemyGunComponent::SpawnManyBullets, 0.001f, false);

	MagazineCapacity--;
	OwningEnemyInterface->PlayShootAnimation();

	AddImpulseDuringShooting();

	bCanShoot = false;
}

void UEnemyGunComponent::ShootEffects()
{
	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, OwningEnemyInterface->GetSkeletalMesh(), MuzzleFleshSocketNames[CurrentSocketNameIndex]);
	if (ShootParticle)
	{
		UParticleSystemComponent* SpawnedShootParticle = UGameplayStatics::SpawnEmitterAttached(ShootParticle, OwningEnemyInterface->GetSkeletalMesh(), MuzzleFleshSocketNames[CurrentSocketNameIndex], FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	}
}

void UEnemyGunComponent::SpawnManyBullets()
{
	if (bManyBulletAtOnce) for (int i = 0; i != HowManyBulletsToSpawn; i++) SpawnBullet();
	else SpawnBullet();
}

void UEnemyGunComponent::SpawnBullet()
{
	FRotator BulletRotation = CalculateBulletRotation();

	FTransform BulletTransform = FTransform(BulletRotation, OwningEnemyInterface->GetSkeletalMesh()->GetSocketLocation(BulletSocketsNames[CurrentSocketNameIndex]));
	ABullet* SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletData.BulletClass, BulletTransform);

	FBulletStruct BulletDataForSpawnedBullet = BulletData;
	BulletDataForSpawnedBullet.Damage = (bManyBulletAtOnce == false ? BulletData.Damage : BulletData.Damage / HowManyBulletsToSpawn);
	BulletDataForSpawnedBullet.HitImpulseForce = (bManyBulletAtOnce == false ? BulletData.HitImpulseForce : BulletData.HitImpulseForce / HowManyBulletsToSpawn);

	SpawnedBullet->SetBulletData(BulletDataForSpawnedBullet);
	SpawnedBullet->FinishSpawning(BulletTransform);

	CurrentSocketNameIndex++;
	if (CurrentSocketNameIndex >= BulletSocketsNames.Num())
	{
		CurrentSocketNameIndex = 0;
	}
}

FVector UEnemyGunComponent::PredictWhereToShoot(bool bIgnoreOffset)
{
	AActor* CurrentFocusedActor = OwningEnemyInterface->GetFocusedActor();
	if (IsValid(CurrentFocusedActor) == false)
		return FVector(0.f);
	FVector FocusedActorLocation = CurrentFocusedActor->GetActorLocation();

	if (bIgnoreOffset == true)
		return FocusedActorLocation;

	float Distance = UKismetMathLibrary::Vector_Distance(GetOwner()->GetActorLocation(), FocusedActorLocation) / PredictWhereToShootDistanceDivider;

	FocusedActorLocation += CurrentFocusedActor->GetActorUpVector() * PredictWhereToShootOffset_UP;
	FocusedActorLocation += CurrentFocusedActor->GetActorRightVector() * CurrentFocusedActor->GetInputAxisValue("Right") * PredictWhereToShootOffset_Right ;
	FocusedActorLocation += CurrentFocusedActor->GetRootComponent()->GetForwardVector() * CurrentFocusedActor->GetInputAxisValue("Forward") * PredictWhereToShootOffset_Forward;
	return FocusedActorLocation;
}

FRotator UEnemyGunComponent::CalculateBulletRotation()
{
	FRotator BulletRotation;
	if (bFindBulletRotationTowardsTarget == true)
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
	FVector RecoilImpulse = -GetOwner()->GetActorForwardVector() * RecoilImpulseOnEnemy;
	OwningEnemyInterface->AddImpulseToPhysicsMesh(RecoilImpulse);
}

void UEnemyGunComponent::Reload()
{
	if (bIsReloading == true)
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

bool UEnemyGunComponent::CanShoot()
{
	if (BulletData.BulletClass == NULL)
		return false;

	if (MagazineCapacity <= 0)
	{
		Reload();
		return false;
	}

	if (bCanShoot == false)
		return false;

	return true;
}


