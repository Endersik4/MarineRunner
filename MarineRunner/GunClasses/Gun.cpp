// Copyright Adam Bartela.All Rights Reserved

#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

#include "MarineRunner/GunClasses/Components/GunRecoilComponent.h"
#include "MarineRunner/GunClasses/Components/DropCasingComponent.h"
#include "MarineRunner/GunClasses/Components/GunControlsComponent.h"
#include "MarineRunner/GunClasses/Components/GunReloadComponent.h"
#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/GunClasses/Scope.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/ArmsSwayComponent.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = false;

	GunSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Skeletal Mesh"));
	RootComponent = GunSkeletalMesh;
	GunSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));

	GunRecoilComponent = CreateDefaultSubobject<UGunRecoilComponent>(TEXT("Gun Recoil Component"));
	DropCasingComponent = CreateDefaultSubobject<UDropCasingComponent>(TEXT("Drop Casing Component"));
	GunControlsComponent = CreateDefaultSubobject<UGunControlsComponent>(TEXT("Gun Controls Component"));
	GunReloadComponent = CreateDefaultSubobject<UGunReloadComponent>(TEXT("Gun Reload Component"));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SetUpZoom();
}

void AGun::SetUpZoom()
{
	if (bUseScope == false)
		return;

	TArray<AActor*> AllChildActors;
	GetAllChildActors(AllChildActors);
	for (AActor* CurrentChildActor : AllChildActors)
	{
		if (!CurrentChildActor->ActorHasTag("Scope"))
			continue;

		ScopeActor = Cast<AScope>(CurrentChildActor);
		break;
	}

	if (!IsValid(ScopeActor))
		return;

	ScopeActor->SetZoomMaterialIndexOnWeapon(ZoomMaterialIndexOnWeapon);
	ScopeActor->SetUpZoomMaterial(this);
}

#pragma region //////////////////////////////////// SHOOT /////////////////////////////////////
bool AGun::CanShoot()
{
	if (!IsValid(BulletData.BulletClass)) 
		return false;

	if (!GunReloadComponent->CanShootWhileReloading())
		return false;

	if (!bCanShoot)
	{
		bCoyoteShootTime = true;
		GetWorldTimerManager().SetTimer(CoyoteShootHandle, this, &AGun::ShootAfterDelayIfCan, CoyoteShootTime, false);

		return false;
	}

	if (GunReloadComponent->GetMagazineCapacity() <= 0)
	{
		if (IsValid(EmptyMagazineSound)) 
			UGameplayStatics::PlaySound2D(GetWorld(), EmptyMagazineSound);
		ShootReleased();

		return false;
	}

	// if the player has released the shoot button before ShootFinished(), the weapon will fire indefinitely, preventing the bug
	if (bAutomaticGun && !bShootButtonPressed)
	{
		ShootReleased();
		return false;
	}

	return true;
}

void AGun::SetCanShootAgain()
{
	bCanShoot = true;
	Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(true);

	if (GunReloadComponent->GetMagazineCapacity() <= 0) 
		GunReloadComponent->PrepareToReload();
}

void AGun::Shoot()
{
	if (CanShoot() == false) 
		return;

	if (bAutomaticGun)
		bConstantlyShoot = true;

	AddEffectsToShooting();

	if (bManyBulletsPerShoot)
	{
		for (int32 i = 0; i != BulletsAmountPerShoot; i++) 
			SpawnBullet();
	}
	else 
		SpawnBullet();

	GunReloadComponent->SetMagazineCapacity(GunReloadComponent->GetMagazineCapacity() - 1);
	GunControlsComponent->UpdateWeaponDataInHud();

	bCanShoot = false;
	GunRecoilComponent->ShootPressed();
	GetWorldTimerManager().SetTimer(ShootHandle, this, &AGun::ShootFinished, ShootTime, false);
}

void AGun::ShootReleased()
{
	bShootButtonPressed = false;
	GunRecoilComponent->ShootReleased();
}

void AGun::ShootFinished()
{
	SetCanShootAgain();

	if (bConstantlyShoot || bCoyoteShootTime)
	{
		bCoyoteShootTime = false;
		bConstantlyShoot = false;
		Shoot();
	}
}

void AGun::CancelShoot()
{
	GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
}
#pragma endregion

#pragma region ////////////////////////////////// BULLET //////////////////////////////////////
void AGun::SpawnBullet()
{
	FVector BulletLocation = GunSkeletalMesh->GetSocketLocation(TEXT("Bullet"));
	FRotator BulletRotation = GetActorRotation() + GunRecoilComponent->RandomBulletRotation();

	FTransform BulletTransform = FTransform(BulletRotation, BulletLocation);
	TObjectPtr<ABullet> SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletData.BulletClass, BulletTransform);

	if (!IsValid(SpawnedBullet))
		return;

	FBulletStruct BulletDataForSpawnedBullet = BulletData;
	BulletDataForSpawnedBullet.Damage = bManyBulletsPerShoot == false ? BulletData.Damage : (BulletData.Damage / BulletsAmountPerShoot);
	BulletDataForSpawnedBullet.HitImpulseForce = bManyBulletsPerShoot == false ? BulletData.HitImpulseForce : (BulletData.HitImpulseForce / BulletsAmountPerShoot);

	SpawnedBullet->SetBulletData(BulletDataForSpawnedBullet);
	SpawnedBullet->FinishSpawning(BulletTransform);
}
#pragma endregion

#pragma region ////////////////////////////////// EFFECTS /////////////////////////////////////
void AGun::AddEffectsToShooting()
{
	if (IsValid(ShootSound))
	{
		UGameplayStatics::SpawnSoundAttached(ShootSound, GunSkeletalMesh, AttachShootEffectsSocketName);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GunSkeletalMesh->GetComponentLocation(), 1.f, this);
	}
	if (ShootParticle) 
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, GunSkeletalMesh, AttachShootEffectsSocketName, FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));

	PlayGunShootAnimation();

	DropCasingComponent->StartTimerToDropCasing();
}

void AGun::PlayGunShootAnimation()
{
	if (NoBulletsShootAnim())
		return;

	if (StatusOfGun == EStatusOfAimedGun::ESAG_ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSShootAnim);
	}
	else
		PlayGivenWeaponWithArmsAnimation(WeaponShootAnim);
}

bool AGun::NoBulletsShootAnim()
{
	if (GunReloadComponent->GetMagazineCapacity() != 1)
		return false;

	if (StatusOfGun == EStatusOfAimedGun::ESAG_ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSShootWithNoBulletsAnim);
	}
	else
		PlayGivenWeaponWithArmsAnimation(WeaponShootWithNoBulletsAnim);

	return true;
}
#pragma endregion

#pragma region ///////////////////////////////// ADS //////////////////////////////////////////
void AGun::AimTheGun(EStatusOfAimedGun NewGunStatus)
{
	StatusOfGun = NewGunStatus;

	if (StatusOfGun == EStatusOfAimedGun::ESAG_ADS)
		Player->GetArmsSwayComponent()->ResetArmsLocation();

	// When in ADS, change weapon sway to other values, or back to original values if player is not in ADS
	Player->GetArmsSwayComponent()->SetArmsSwayDivider(StatusOfGun == EStatusOfAimedGun::ESAG_ADS ? WeaponSwayInADSDivider : 1.f);
	Player->GetArmsSwayComponent()->SetArmsSwayWhileMovingDivider(StatusOfGun == EStatusOfAimedGun::ESAG_ADS ? WeaponSwayWhileMovingInADSDivider : 1);
	
	// Player cant change weapon when in ADS
	Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(!StatusOfGun == EStatusOfAimedGun::ESAG_ADS);

	if (bUseScope == true)
		ScopeActor->ActiveZoom(StatusOfGun == EStatusOfAimedGun::ESAG_ADS);

	if (StatusOfGun == EStatusOfAimedGun::ESAG_ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSInAnim);
	}
	else 
		PlayGivenWeaponWithArmsAnimation(WeaponADSOutAnim);

}
#pragma endregion

void AGun::PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const
{
	if (AnimToPlay.WeaponActionAnim)
	{
		GunSkeletalMesh->PlayAnimation(AnimToPlay.WeaponActionAnim, false);
	}

	if (AnimToPlay.ArmsActionAnim)
	{
		Player->GetArmsSkeletalMesh()->PlayAnimation(AnimToPlay.ArmsActionAnim, false);
	}
}
