// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Gun/Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

#include "MarineRunner/Gun/Components/GunRecoilComponent.h"
#include "MarineRunner/Gun/Components/DropCasingComponent.h"
#include "MarineRunner/Gun/Components/GunControlsComponent.h"
#include "MarineRunner/Gun/Components/GunReloadComponent.h"
#include "MarineRunner/Gun/Bullet/Bullet.h"
#include "MarineRunner/Gun/Mods/ScopeGunMod.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/Components/ArmsSwayComponent.h"

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
	if (!bUseScope)
		return;

	TArray<TObjectPtr<AActor>> AllChildActors;
	GetAllChildActors(AllChildActors);
	for (TObjectPtr<AActor> CurrentChildActor : AllChildActors)
	{
		if (!IsValid(CurrentChildActor))
			continue;

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
		else
			UE_LOG(LogTemp, Warning, TEXT("Empty Magazine Sound is nullptr in Gun!"));

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
	if (!CanShoot()) 
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
	{
		SpawnBullet();
	}

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
	if (!IsValid(BulletData.BulletClass))
		return;

	const FVector& BulletLocation = GunSkeletalMesh->GetSocketLocation(TEXT("Bullet"));
	const FRotator& BulletRotation = GetActorRotation() + GunRecoilComponent->RandomBulletRotation();

	const FTransform& BulletTransform = FTransform(BulletRotation, BulletLocation);
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Shoot Sound is nullptr in Gun!"));
	}

	if (ShootParticle) 
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, GunSkeletalMesh, AttachShootEffectsSocketName, FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	else
		UE_LOG(LogTemp, Warning, TEXT("Shoot Particle is nullptr in Gun!"));

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
	{
		PlayGivenWeaponWithArmsAnimation(WeaponShootAnim);
	}
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
	{
		PlayGivenWeaponWithArmsAnimation(WeaponShootWithNoBulletsAnim);
	}

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

	if (bUseScope)
		ScopeActor->ActiveZoom(StatusOfGun == EStatusOfAimedGun::ESAG_ADS);

	if (StatusOfGun == EStatusOfAimedGun::ESAG_ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSInAnim);
	}
	else
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSOutAnim);
	}

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

#pragma region ///// Weapon Interface Functions ///////

void AGun::DrawWeapon()
{
	GunControlsComponent->DrawGun();
}

void AGun::TakeWeapon(AMarineCharacter* PlayerTemp, bool bWasOnceTaken, int32 CurrentMagazineCapacityToLoad)
{
	AttachToComponent(PlayerTemp->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunControlsComponent->GetAttachToSocketName());
	GunControlsComponent->TakeGun(PlayerTemp, bWasOnceTaken, CurrentMagazineCapacityToLoad);
}

void AGun::PutAwayWeapon()
{
	GunControlsComponent->PutAwayGun();
}

void AGun::SetDropWeapon(bool bDrop)
{
	GunControlsComponent->SetDropGun(bDrop);
}

void AGun::HideWeapon()
{
	GunControlsComponent->HideGun();
}

void AGun::PrimaryAction()
{
	SetShootButtonPressed(true);
	Shoot();
}

void AGun::ReleasedPrimaryAction()
{
	ShootReleased();
}

void AGun::SecondaryAction()
{
	if (GunReloadComponent->GetIsReloading() && GunReloadComponent->GetReloadOneBullet())
		GunReloadComponent->CancelReload();

	if (!GetCanShoot())
		return;

	Player->MakeCrosshire(true);
	Player->SetMovementForceDividerWhenInADS(MovementForceDividerWhenInADS);

	if (IsValid(ADSInSound))
		UGameplayStatics::SpawnSound2D(GetWorld(), ADSInSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("ADS In Sound is nullptr in Gun!"));

	Player->GetWeaponHandlerComponent()->SetIsPlayerInAds(true);
	AimTheGun(EStatusOfAimedGun::ESAG_ADS);

	if (CurrentScopeIndex >= Player->GetWeaponHandlerComponent()->GetMouseSensitivityWhenScope().Num())
		return;

	Player->ChangeMouseSensitivity(Player->GetWeaponHandlerComponent()->GetMouseSensitivityWhenScope()[CurrentScopeIndex]);
}

void AGun::ReleasedSecondaryAction()
{
	if (!Player->GetWeaponHandlerComponent()->GetIsPlayerInAds())
		return;

	Player->MakeCrosshire();
	Player->SetMovementForceDividerWhenInADS(1.f);

	if (IsValid(ADSOutSound))
		UGameplayStatics::SpawnSound2D(GetWorld(), ADSOutSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("ADS Out Sound is nullptr in gun!"));

	Player->GetWeaponHandlerComponent()->SetIsPlayerInAds(false);

	AimTheGun(EStatusOfAimedGun::ESAG_HipFire);

	Player->ChangeMouseSensitivity(FSettingSavedInJsonFile(), true);

	if (GetUseScope())
		CurrentScopeIndex = GetScopeActor()->Zoom(0.f, true);
	else
		CurrentScopeIndex = 0;
}

void AGun::TertiaryAction(float Value)
{
	if (!Player->GetWeaponHandlerComponent()->GetIsPlayerInAds() || Value == 0.f)
		return;

	if (!GetUseScope())
		return;

	if (!IsValid(GetScopeActor()))
		return;

	CurrentScopeIndex = GetScopeActor()->Zoom(Value);

	if (CurrentScopeIndex >= Player->GetWeaponHandlerComponent()->GetMouseSensitivityWhenScope().Num())
		return;

	Player->ChangeMouseSensitivity(Player->GetWeaponHandlerComponent()->GetMouseSensitivityWhenScope()[CurrentScopeIndex]);
}

void AGun::ActionFromKey_One()
{
	GunReloadComponent->PrepareToReload();
}

void AGun::UpdateWeaponHudInformation()
{
	GunControlsComponent->UpdateWeaponDataInHud(true);
}

FString AGun::GetPathToWeaponClass()
{
	return GetClass()->GetClassPathName().ToString();
}

int32 AGun::GetIntValueToSave()
{
	return GunReloadComponent->GetMagazineCapacity();
}
#pragma endregion
