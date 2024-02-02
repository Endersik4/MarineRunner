// Copyright Adam Bartela.All Rights Reserved

#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/GunClasses/Scope.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/ArmsSwayComponent.h"

#include "MarineRunner/Inventory/PickupItem.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Skeletal Mesh"));
	RootComponent = GunSkeletalMesh;

	GunSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));

	Tags.Add(TEXT("Gun"));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SetupFloatTimeline(&RecoilGunTimeline, FName(TEXT("ShootRecoilTimelineProgress")), FName(TEXT("ShootRecoilTimelineFinished")), ShootFOVCurve);
	
	if (bShouldUseCurveRecoil)
		SetupFloatTimeline(&RecoilCameraTimeline, FName(TEXT("RecoilCameraTimelineCallback")), FName(TEXT("RecoilCameraTimelineFinishedCallback")), RecoilCameraCurveY);

	OriginalMagazineCapacity = MagazineCapacity;
	PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

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
		if (CurrentChildActor->ActorHasTag("Scope") == false)
			continue;

		ScopeActor = Cast<AScope>(CurrentChildActor);
		break;
	}

	if (IsValid(ScopeActor) == false)
		return;

	ScopeActor->SetZoomMaterialIndexOnWeapon(ZoomMaterialIndexOnWeapon);
	ScopeActor->SetUpZoomMaterial(this);
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpRecoilCamera(DeltaTime);
	CameraInterpBackToInitialPosition(DeltaTime);

	RecoilGunTimeline.TickTimeline(DeltaTime);
	RecoilCameraTimeline.TickTimeline(DeltaTime);
}

#pragma region //////////////////////////////////// SHOOT /////////////////////////////////////
bool AGun::CanShoot()
{
	if (BulletData.BulletClass == NULL) 
		return false;

	if (bReloadOneBullet && bIsReloading && MagazineCapacity > 0)
	{
		CancelReload();
		return true;
	}
	else if (bIsReloading) return false;

	return true;
}

void AGun::SetCanShoot()
{
	bCanShoot = true;
	MarinePawn->GetWeaponHandlerComponent()->SetCanChangeWeapon(true);
	if (MagazineCapacity <= 0) WaitToReload();
}

void AGun::Shoot()
{
	if (CanShoot() == false) return;

	if (bIsAutomatic) 
		bConstantlyShoot = true;

	if (bCanShoot == false)
	{
		bShouldDelayShoot = true;
		GetWorldTimerManager().SetTimer(DelayShootHandle, this, &AGun::DelayShoot, DelayShootTime, false);

		return;
	}


	if (MagazineCapacity <= 0)
	{
		if (EmptyMagazineSound) UGameplayStatics::SpawnSoundAttached(EmptyMagazineSound, GunSkeletalMesh, NAME_None);
		ShootReleased();

		return;
	}

	AddEffectsToShooting();

	if (bManyBulletAtOnce) for (int i = 0; i != HowManyBulletsToSpawn; i++) SpawnBullet();
	else SpawnBullet();

	MagazineCapacity--;
	UpdateWeaponDataInHud();

	bCanShoot = false;

	if (bShouldWaitToPlayRecoil == true) GetWorldTimerManager().SetTimer(PlayRecoilHandle, this, &AGun::PlayRecoil, PlayRecoilTime, false);
	else PlayRecoil();
}

void AGun::ShootReleased()
{
	if (bCanRecoilCamera == false) return;

	GetWorldTimerManager().SetTimer(FirstBulletHandle, this, &AGun::ShouldFirstBulletGoStraight, FirstBulletWithoutRecoilTime, false);
	ResetVariablesForCameraRecoil();
	if (bShouldUseCurveRecoil) BackCameraToItsInitialRotation();
}
#pragma endregion

#pragma region ////////////////////////////////// RECOIL //////////////////////////////////////
void AGun::PlayRecoil()
{
	SetCameraRecoil(); //Recoil CAMERA
	RecoilGunTimeline.PlayFromStart();
}
#pragma endregion

#pragma region //////////////////////////////// GUN RECOIL ////////////////////////////////////
void AGun::ShootRecoilTimelineProgress(float RecoilDirection)
{
	if (RecoilCameraCurveRandomRotation)
	{
		float ControlRotationPitch = RandomRecoilPitch * RecoilCameraCurveRandomRotation->GetFloatValue(RecoilGunTimeline.GetPlaybackPosition());
		float ControlRotationYaw = RandomRecoilYaw * RecoilCameraCurveRandomRotation->GetFloatValue(RecoilGunTimeline.GetPlaybackPosition());
		
		PC->AddYawInput(ControlRotationYaw);
		PC->AddPitchInput(-ControlRotationPitch);
	}

	MarinePawn->GetCamera()->FieldOfView += RecoilDirection;
}

void AGun::ShootRecoilTimelineFinished()
{
	MarinePawn->GetCamera()->FieldOfView = OriginalPlayerFOV;

	SetCanShoot();

	if (bConstantlyShoot)
		Shoot();

	if (bShouldDelayShoot) bShouldDelayShoot = false;
}
#pragma endregion

#pragma region ////////////////////////////// CAMERA RECOIL ///////////////////////////////////
void AGun::RecoilCameraTimelineCallback(float ControlRotationY)
{
	//Randomize recoil a bit
	ControlRotationY += RandomValueForCameraYRecoil;

	if (StatusOfGun == ADS)
		ControlRotationY /= DividerOfRecoilWhileADS;

	PC->AddYawInput(ControlRotationY);
}

void AGun::SetCameraRecoil()
{
	if (bCanRecoilCamera == true) return;

	InitialCameraRotation = PC->GetControlRotation();

	if (bShouldUseCurveRecoil)
	{
		RecoilCameraTimeline.PlayFromStart();
		RandomValueForCameraYRecoil = FMath::FRandRange(RandomRangeFromRecoilCurveY.GetLowerBoundValue(), RandomRangeFromRecoilCurveY.GetUpperBoundValue());

		bCanRecoilCamera = true;

		return;
	}

	RandomRecoilYaw = FMath::FRandRange(YawRecoilRangeArray.GetLowerBoundValue(), YawRecoilRangeArray.GetUpperBoundValue());
	RandomRecoilPitch = FMath::FRandRange(PitchRecoilRangeArray.GetLowerBoundValue(), PitchRecoilRangeArray.GetUpperBoundValue());
	if (StatusOfGun == ADS)
	{
		RandomRecoilYaw /= DividerOfRecoilWhileADS;
		RandomRecoilPitch /= DividerOfRecoilWhileADS;
	}
}

void AGun::UpRecoilCamera(float Delta)
{
	if (bCanRecoilCamera == false) return;

	if (bShouldUseCurveRecoil)
	{
		float ControlRotationPitch = (DistanceFromStart * 0.375) * TimeRecoilCameraElapsed / ((OriginalMagazineCapacity * RecoilAnimTimelineLength) + 0.2f);
		if (StatusOfGun == ADS)
		{
			ControlRotationPitch /= DividerOfRecoilWhileADS;
		}
		PC->AddPitchInput(-ControlRotationPitch);
	}
	TimeRecoilCameraElapsed = Delta;
}

void AGun::BackCameraToItsInitialRotation()
{
	FRotator CurrentControlRotation = PC->GetControlRotation();

	//Calculating Distance between two pitches
	float DistanceBetweenPitch;
	DistanceBetweenPitch = FMath::Abs((InitialCameraRotation.Pitch - CurrentControlRotation.Pitch));
	if ((InitialCameraRotation.Pitch < 100.f && CurrentControlRotation.Pitch >= 100.f) || (CurrentControlRotation.Pitch < 100.f && InitialCameraRotation.Pitch >= 100.f))
	{
		DistanceBetweenPitch = 360.f - DistanceBetweenPitch;
	}

	if (bShouldUseCurveRecoil)
	{
		UKismetMathLibrary::NormalizedDeltaRotator(PC->GetControlRotation(), InitialCameraRotation);
		//If distance is too big then camera doesnt go back to its inital rotation
		float DistanceBetweenYaw = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(PC->GetControlRotation(), InitialCameraRotation).Yaw);
		if (DistanceBetweenYaw > 10.f)
		{
			InitialCameraRotation.Yaw = PC->GetControlRotation().Yaw;
		}
		if (DistanceBetweenPitch > DistanceFromStart)
		{
			InitialCameraRotation = PC->GetControlRotation();
			InitialCameraRotation.Pitch -= DistanceFromStart;
		}
	}

	bShouldCameraInterpBack = true;
}

void AGun::CameraInterpBackToInitialPosition(float Delta)
{
	if (!MarinePawn) return;

	if (bShouldCameraInterpBack == false) return;

	// If the player moves the mouse, it stops the camera movement that was going back to its starting position.
	if (PC->MouseXValue > 0.05f || PC->MouseXValue < -0.05f || PC->MouseYValue > 0.05f || PC->MouseYValue < -0.05f)
	{
		bShouldCameraInterpBack = false;
		return;
	}

	if (PC->GetControlRotation().Equals(InitialCameraRotation, 0.05f))
	{
		bShouldCameraInterpBack = false;
		return;
	}

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(PC->GetControlRotation(), InitialCameraRotation, Delta, InitalCameraPositionSpeed);
	PC->SetControlRotation(NewRotation);
}

void AGun::ResetVariablesForCameraRecoil()
{
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	bCanRecoilCamera = false;
	TimeRecoilCameraElapsed = 0.f;
	bConstantlyShoot = false;
	if (bShouldUseCurveRecoil) RecoilCameraTimeline.Stop();
}
#pragma endregion

#pragma region ////////////////////////////////// BULLET //////////////////////////////////////
void AGun::SpawnBullet()
{
	//Proper Location and Rotation for Bullet
	FVector Location = GunSkeletalMesh->GetSocketLocation(TEXT("Bullet"));
	BulletRotation = GetActorRotation();

	RandomBulletDirection(BulletRotation);

	bFirstBulletWithoutRecoil = false;

	FTransform BulletTransform = FTransform(BulletRotation, Location);
	ABullet* SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletData.BulletClass, BulletTransform);

	FBulletStruct BulletDataForSpawnedBullet = BulletData;
	BulletDataForSpawnedBullet.Damage = (bManyBulletAtOnce == false ? BulletData.Damage : BulletData.Damage / HowManyBulletsToSpawn);
	BulletDataForSpawnedBullet.HitImpulseForce = (bManyBulletAtOnce == false ? BulletData.HitImpulseForce : BulletData.HitImpulseForce / HowManyBulletsToSpawn);

	SpawnedBullet->SetBulletData(BulletDataForSpawnedBullet);
	SpawnedBullet->FinishSpawning(BulletTransform);
}

void AGun::RandomBulletDirection(FRotator& NewBulletRotation)
{
	if (bFirstBulletWithoutRecoil == true)
		return;

	float NewPitchBulletRotaton = FMath::FRandRange(PitchBulletRecoilArray.GetLowerBoundValue(), PitchBulletRecoilArray.GetUpperBoundValue());
	float NewYawBulletRotation = FMath::FRandRange(YawBulletRecoilArray.GetLowerBoundValue(), YawBulletRecoilArray.GetUpperBoundValue());
	if (StatusOfGun == ADS)
	{
		NewPitchBulletRotaton /= DividerOfBulletRecoilWhileADS;
		NewYawBulletRotation /= DividerOfBulletRecoilWhileADS;
	}

	BulletRotation.Pitch += NewPitchBulletRotaton;
	BulletRotation.Yaw += NewYawBulletRotation;
}
#pragma endregion

#pragma region ////////////////////////////////// RELOAD //////////////////////////////////////
bool AGun::CanReload()
{
	if (GetPointerToAmmoFromInventory() == false || MagazineCapacity == OriginalMagazineCapacity || GetWorldTimerManager().IsTimerActive(ReloadHandle))
	{
		bIsReloading = false;
		return false;
	}
	if (AmmunitionFromInventory->Item_Amount <= 0)
	{
		bIsReloading = false;
		return false;
	}

	return true;
}

void AGun::WaitToReload()
{
	if (CanReload() == false) 
		return;

	bCanShoot = false;
	ShootReleased();

	MarinePawn->GetWeaponHandlerComponent()->ADSReleased();

	if (bCasingEjectionWhileReloading == true) DropCasing();

	const FWeaponAnimation& ReloadAnimToPlay = ReloadAnimAccordingToSituation();
	PlayGivenWeaponWithArmsAnimation(ReloadAnimToPlay);

	bIsReloading = true;

	GetWorldTimerManager().SetTimer(ReloadHandle, this, &AGun::Reload, ReloadAnimToPlay.ArmsActionAnim->GetPlayLength(), false);
}

const FWeaponAnimation& AGun::ReloadAnimAccordingToSituation()
{
	if (MagazineCapacity == 0)
		return WeaponReloadWithNoBulletsAnim;

	if (bReloadOneBullet == true && bIsReloading == false && MagazineCapacity == OriginalMagazineCapacity - 1 || AmmunitionFromInventory->Item_Amount <= 1)
		return WeaponReload_BeginEnd;

	if (bReloadOneBullet == true && bIsReloading == false)
		return WeaponReload_Begin;

	if (bReloadOneBullet == true && bIsReloading == true && MagazineCapacity == OriginalMagazineCapacity - 1 || AmmunitionFromInventory->Item_Amount <= 1)
		return WeaponReload_End;

	return WeaponReloadAnim;
}

void AGun::Reload()
{
	if (GetPointerToAmmoFromInventory() == false) return;

	RemoveAmmunitionFromInventory();

	bCanShoot = true;
	UpdateWeaponDataInHud(true);

	if (AmmunitionFromInventory->Item_Amount <= 0) MarinePawn->GetInventoryComponent()->Inventory_Items.Remove(SpawnedAmmoItemData.Item_Name);
	MarinePawn->UpdateAlbertosInventory();

	GetWorldTimerManager().ClearTimer(ReloadHandle);
	if (bReloadOneBullet) 
		WaitToReload();
	else 
		bIsReloading = false;

}

void AGun::CancelReload()
{
	GetWorldTimerManager().ClearTimer(ReloadHandle);

	bCanShoot = true;
	bIsReloading = false;
}

void AGun::RemoveAmmunitionFromInventory()
{
	if (bReloadOneBullet == true)
	{
		MagazineCapacity++;
		AmmunitionFromInventory->Item_Amount--;
	}
	else
	{
		int32 RestAmmo = OriginalMagazineCapacity - MagazineCapacity;
		if (AmmunitionFromInventory->Item_Amount < RestAmmo)
		{
			MagazineCapacity += AmmunitionFromInventory->Item_Amount;
			AmmunitionFromInventory->Item_Amount = 0;
		}
		else
		{
			AmmunitionFromInventory->Item_Amount -= RestAmmo;
			MagazineCapacity = OriginalMagazineCapacity;
		}
	}
}
#pragma endregion

#pragma region ////////////////////////////////// EFFECTS /////////////////////////////////////
void AGun::AddEffectsToShooting()
{
	if (ShootingSound) 
		UGameplayStatics::SpawnSoundAttached(ShootingSound, GunSkeletalMesh, NAME_None);
	if (ShootParticle) 
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, GunSkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));

	PlayGunShootAnimation();

	if (bCasingEjectionWhileReloading == false) DropCasing();
}

void AGun::PlayGunShootAnimation()
{
	if (NoBulletsShootAnim() == true)
		return;

	if (StatusOfGun == EStatusOfAimedGun::ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSShootAnim);
	}
	else
		PlayGivenWeaponWithArmsAnimation(WeaponShootAnim);
}

bool AGun::NoBulletsShootAnim()
{
	if (MagazineCapacity != 1)
		return false;

	if (StatusOfGun == EStatusOfAimedGun::ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSShootWithNoBulletsAnim);
	}
	else
		PlayGivenWeaponWithArmsAnimation(WeaponShootWithNoBulletsAnim);

	return true;
}

void AGun::DropCasing()
{
	if (DropBulletClass == nullptr) 
		return;

	FRotator DropBulletRotation = GetActorRotation();
	if (bShouldRandomizeRotationOfCasing)
	{
		DropBulletRotation.Yaw -= FMath::FRandRange(-10.f, 40.f);
		DropBulletRotation.Roll += FMath::FRandRange(-10.f, 10.f);
		DropBulletRotation.Pitch += FMath::FRandRange(-15.f, 15.f);
	}

	AActor* DropBullet = GetWorld()->SpawnActor<AActor>(DropBulletClass, GunSkeletalMesh->GetSocketLocation(TEXT("BulletDrop")), DropBulletRotation);
	if (IsValid(DropBullet) == false)
		return;

	DropBullet->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}
#pragma endregion

#pragma region ///////////////////////////////// ADS //////////////////////////////////////////
void AGun::AimTheGun(EStatusOfAimedGun NewGunStatus)
{
	StatusOfGun = NewGunStatus;

	MarinePawn->GetArmsSwayComponent()->SetInADS(StatusOfGun == EStatusOfAimedGun::ADS);

	// When in ADS, change weapon sway to other values, or back to original values if player is not in ADS
	MarinePawn->GetArmsSwayComponent()->SetWeaponSwayDivider(StatusOfGun == EStatusOfAimedGun::ADS ? WeaponSwayInADSDivider : 1.f);
	MarinePawn->GetArmsSwayComponent()->SetWeaponSwayWhileMovingDivider(StatusOfGun == EStatusOfAimedGun::ADS ? WeaponSwayWhileMovingInADSDivider : 1);
	
	// Player cant change weapon when in ADS
	MarinePawn->GetWeaponHandlerComponent()->SetCanChangeWeapon(!StatusOfGun == EStatusOfAimedGun::ADS);

	if (bUseScope == true)
	{
		ScopeActor->ActiveZoom(StatusOfGun == EStatusOfAimedGun::ADS);
	}

	if (StatusOfGun == EStatusOfAimedGun::ADS)
	{
		PlayGivenWeaponWithArmsAnimation(WeaponADSInAnim);
	}
	else 
		PlayGivenWeaponWithArmsAnimation(WeaponADSOutAnim);
}

#pragma endregion

#pragma region ///////////////////////////////// HUD //////////////////////////////////////////
void AGun::UpdateWeaponDataInHud(bool bChangeStoredAmmoText, bool bChangeWeaponImage)
{
	if (!HudWidget) return;

	HudWidget->SetAmmoText(MagazineCapacity);

	if (bChangeStoredAmmoText && GetPointerToAmmoFromInventory()) HudWidget->SetAmmoText(AmmunitionFromInventory->Item_Amount, true);
	if (bChangeWeaponImage)
	{
		HudWidget->SetWeaponImage(GunHUDTexture, bAmmoCounterBelowGunHUD);
	}
}
#pragma endregion

#pragma region ////////////////////////////////// TAKE ////////////////////////////////////////
void AGun::TakeGun(AMarineCharacter* Player)
{
	if (IsValid(Player) == false)
		return;

	MarinePawn = Player;
	bCanShoot = false;
	MarinePawn->GetWeaponHandlerComponent()->HideCurrentHoldingGun();
	MarinePawn->GetWeaponHandlerComponent()->SetCanChangeWeapon(false);

	PlayGivenWeaponWithArmsAnimation(WeaponFirstTimeTakeAnim);

	OriginalPlayerFOV = MarinePawn->GetCamera()->FieldOfView;

	SpawnAmmunitionObjectForVariables();
	AddAmmoToInventory();
	MarinePawn->GetWeaponHandlerComponent()->SetGun(this);
	MarinePawn->GetWeaponInventoryComponent()->AddNewWeaponToStorage(this);

	HudWidget = MarinePawn->GetHudWidget();
	HudWidget->ShowWeaponOnHud();
	UpdateWeaponDataInHud(true, true);

	FTimerHandle TakeGunHandle;
	GetWorld()->GetTimerManager().SetTimer(TakeGunHandle, this, &AGun::SetCanShoot, WeaponFirstTimeTakeAnim.ArmsActionAnim->GetPlayLength(), false);
}

void AGun::DrawGun()
{
	if (IsValid(MarinePawn) == false)
		return;

	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	bCanShoot = false;

	MarinePawn->GetWeaponHandlerComponent()->SetGun(this);

	PlayGivenWeaponWithArmsAnimation(WeaponDrawAnim);

	if (DrawGunSound) UGameplayStatics::SpawnSound2D(GetWorld(), DrawGunSound);

	HudWidget->ShowWeaponOnHud();
	UpdateWeaponDataInHud(true, true);

	FTimerHandle DrawGunHandle;
	GetWorld()->GetTimerManager().SetTimer(DrawGunHandle, this, &AGun::SetCanShoot, WeaponDrawAnim.ArmsActionAnim->GetPlayLength(), false);
}

void AGun::PutAwayGun()
{
	if (IsValid(MarinePawn) == false)
		return;

	bCanShoot = false;

	MarinePawn->GetWeaponHandlerComponent()->SetGun(nullptr);

	PlayGivenWeaponWithArmsAnimation(WeaponHideAnim);

	if (bIsReloading == true) 
		CancelReload();
	ShootReleased();

	if (PutAwayGunSound) 
		UGameplayStatics::SpawnSound2D(GetWorld(), PutAwayGunSound);

	HudWidget->ShowWeaponOnHud(false);

	FTimerHandle HideGunHandle;
	GetWorld()->GetTimerManager().SetTimer(HideGunHandle, this, &AGun::HideGun, WeaponHideAnim.ArmsActionAnim->GetPlayLength(), false);
}

void AGun::HideGun()
{
	bCanShoot = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	MarinePawn->GetWeaponHandlerComponent()->DrawNewGun();
}

#pragma endregion

#pragma region ////////////////////////////////// DROP ////////////////////////////////////////
#pragma endregion

void AGun::PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const
{
	if (AnimToPlay.WeaponActionAnim)
	{
		GunSkeletalMesh->PlayAnimation(AnimToPlay.WeaponActionAnim, false);
	}

	if (AnimToPlay.ArmsActionAnim)
	{
		MarinePawn->GetArmsSkeletalMesh()->PlayAnimation(AnimToPlay.ArmsActionAnim, false);
	}
}

#pragma region /////////////////////////////// INVENTORY //////////////////////////////////////
void AGun::SpawnAmmunitionObjectForVariables()
{
	// TODO: Change Whole Spawning Object system to Data Tables
	APickupItem* SpawnedAmmunition = GetWorld()->SpawnActor<APickupItem>(AmmunitionItemClass, FVector(0.f), FRotator(0.f));
	if (SpawnedAmmunition == nullptr) return;

	SpawnedAmmoItemData = SpawnedAmmunition->GetItemSettings();
	SpawnedAmmoItemData.Item_Amount = 0;

	if (StoredAmmo != 0) SpawnedAmmoItemData.Item_Amount += StoredAmmo;
	SpawnedAmmunition->Destroy();
}

void AGun::AddAmmoToInventory()
{
	if (MarinePawn->GetInventoryComponent() == nullptr) 
		return;

	if (GetPointerToAmmoFromInventory())
	{
		AmmunitionFromInventory->Item_Amount += StoredAmmo;
	}
	else
	{
		MarinePawn->GetInventoryComponent()->Inventory_Items.Add(SpawnedAmmoItemData.Item_Name, SpawnedAmmoItemData);
		AmmunitionFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(SpawnedAmmoItemData.Item_Name);
	}
}

bool AGun::GetPointerToAmmoFromInventory()
{
	AmmunitionFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(SpawnedAmmoItemData.Item_Name);
	return AmmunitionFromInventory ? true : false;
}
#pragma endregion

void AGun::SetupFloatTimeline(FTimeline* TimelineToCreate, FName TimelineProgressFuncName, FName TimelineFinishedFuncName, UCurveFloat* CurveForTimeline)
{
	FOnTimelineFloat TimelineFloatProgress;
	TimelineFloatProgress.BindUFunction(this, TimelineProgressFuncName);
	TimelineToCreate->AddInterpFloat(CurveForTimeline, TimelineFloatProgress);

	FOnTimelineEventStatic TimelineCallbackFinished;
	TimelineCallbackFinished.BindUFunction(this, TimelineFinishedFuncName);
	TimelineToCreate->SetTimelineFinishedFunc(TimelineCallbackFinished);
}