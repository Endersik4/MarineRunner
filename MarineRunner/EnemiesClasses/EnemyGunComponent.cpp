// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/EnemyGunComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/EnemiesClasses/EnemyInterface.h"


// Sets default values for this component's properties
UEnemyGunComponent::UEnemyGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyGunComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningEnemyInterface = Cast<IEnemyInterface>(GetOwner());
	InitialMagazineCapacity = MagazineCapacity;

}


// Called every frame
void UEnemyGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyGunComponent::Shoot()
{
	if (BulletData.BulletClass == NULL || bCanShoot == false || OwningEnemyInterface->GetIsDead() == true) return;

	if (MagazineCapacity <= 0)
	{
		Reload();
		return;
	}

	ShootEffects();

	//Had to do this in this way because bullet was spawned before bones were on their locations (in shooting animation)
	GetWorld()->GetTimerManager().SetTimer(ImpulseOnBulletHandle, this, &UEnemyGunComponent::SpawnManyBullets, 0.001f, false);

	MagazineCapacity--;
	OwningEnemyInterface->PlayShootAnimation();
	bCanShoot = false;

	AddImpulseDuringShooting();
}

void UEnemyGunComponent::ShootEffects()
{
	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, OwningEnemyInterface->GetSkeletalMesh(), TEXT("MuzzleFlash"));
	if (ShootParticle)
	{
		UParticleSystemComponent* SpawnedShootParticle = UGameplayStatics::SpawnEmitterAttached(ShootParticle, OwningEnemyInterface->GetSkeletalMesh(), TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	}
}

void UEnemyGunComponent::SpawnManyBullets()
{
	if (bManyBulletAtOnce) for (int i = 0; i != HowManyBulletsToSpawn; i++) SpawnBullet();
	else SpawnBullet();
}

void UEnemyGunComponent::SpawnBullet()
{
	FRotator BulletRotation = UKismetMathLibrary::FindLookAtRotation(OwningEnemyInterface->GetSkeletalMesh()->GetSocketLocation(BoneNameForBulletRotation), OwningEnemyInterface->GetPlayerCameraLocation());
	//Bullet will randomly "go" to other directions 
	if (PitchBulletRecoilArray.Num() == 2 && YawBulletRecoilArray.Num() == 2 && bManyBulletAtOnce)
	{
		float NewPitchRotaton = FMath::FRandRange(PitchBulletRecoilArray[0], PitchBulletRecoilArray[1]);
		float NewYawRotation = FMath::FRandRange(YawBulletRecoilArray[0], YawBulletRecoilArray[1]);
		BulletRotation.Pitch += NewPitchRotaton;
		BulletRotation.Yaw += NewYawRotation;
	}

	FTransform BulletTransform = FTransform(BulletRotation, OwningEnemyInterface->GetSkeletalMesh()->GetSocketLocation(TEXT("Bullet")));
	ABullet* SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletData.BulletClass, BulletTransform);

	FBulletStruct BulletDataForSpawnedBullet = BulletData;
	BulletDataForSpawnedBullet.Damage = (bManyBulletAtOnce == false ? BulletData.Damage : BulletData.Damage / HowManyBulletsToSpawn);
	BulletDataForSpawnedBullet.HitImpulseForce = (bManyBulletAtOnce == false ? BulletData.HitImpulseForce : BulletData.HitImpulseForce / HowManyBulletsToSpawn);

	SpawnedBullet->SetBulletData(BulletDataForSpawnedBullet);
	SpawnedBullet->FinishSpawning(BulletTransform);
}

void UEnemyGunComponent::AddImpulseDuringShooting()
{
	FVector RecoilImpulse = -GetOwner()->GetActorForwardVector() * RecoilImpulseOnEnemy * 100.f;
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

const bool UEnemyGunComponent::bCanShootAgain()
{
	bCanShoot = true;
	return (bIsReloading == false && MagazineCapacity > 0);
}


