// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Skeletal Mesh"));
	RootComponent = BaseSkeletalMesh;

	BaseSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));

	Tags.Add(TEXT("Gun"));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SpawnAmmunitionObjectForVariables();

	SetupFloatTimeline(&RecoilGunTimeline, FName(TEXT("ShootRecoilTimelineProgress")), FName(TEXT("ShootRecoilTimelineFinished")));
	
	if (bShouldUseCurveRecoil)
		SetupFloatTimeline(&RecoilCameraTimeline, FName(TEXT("RecoilCameraTimelineCallback")), FName(TEXT("RecoilCameraTimelineFinishedCallback")));

	OriginalMagazineCapacity = MagazineCapacity;
	PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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
	if (BulletData.BulletClass == NULL) return false;
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
	if (MagazineCapacity <= 0) WaitToReload();
}

void AGun::Shoot()
{
	if (CanShoot() == false) return;

	if (bIsAutomatic) bConstantlyShoot = true;
	if (bCanShoot == false || BaseSkeletalMesh->IsPlaying() == true)
	{
		bShouldDelayShoot = true;
		GetWorldTimerManager().SetTimer(DelayShootHandle, this, &AGun::DelayShoot, DelayShootTime, false);
		return;
	}

	if (MagazineCapacity <= 0)
	{
		if (EmptyMagazineSound) UGameplayStatics::SpawnSoundAttached(EmptyMagazineSound, BaseSkeletalMesh, NAME_None);
		ShootReleased();
		return;
	}

	//Effect like paricles, sounds or drop casing from weapon
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
	MarinePawn->GetCamera()->FieldOfView = CopyOfFOV;

	if (bPlayShootAnimationAfterFire)
	{
		if (AfterShootSound) UGameplayStatics::SpawnSoundAttached(AfterShootSound, BaseSkeletalMesh, NAME_None);
		BaseSkeletalMesh->SetForceRefPose(false);
		//if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);

		FTimerHandle DropCasingHandle;
		GetWorldTimerManager().SetTimer(DropCasingHandle, this, &AGun::DropCasing, 0.2f, false);

		//FTimerHandle PlayAfterFireHandle;
		//GetWorldTimerManager().SetTimer(PlayAfterFireHandle, this, &AGun::SetCanShoot, ShootAnimation->GetPlayLength(), false);

	}
	else SetCanShoot();

	if (bShouldDelayShoot || bConstantlyShoot)
	{
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AGun::Shoot, 0.015f);
		if (bShouldDelayShoot) bShouldDelayShoot = false;
	}
}
#pragma endregion

#pragma region ////////////////////////////// CAMERA RECOIL ///////////////////////////////////
void AGun::RecoilCameraTimelineCallback(float ControlRotationY)
{
	//Randomize recoil a bit
	ControlRotationY += RandomValueForCameraYRecoil;
	PC->AddYawInput(ControlRotationY);
}

void AGun::SetCameraRecoil()
{
	if (bCanRecoilCamera == true) return;

	InitialCameraRotation = PC->GetControlRotation();

	if (bShouldUseCurveRecoil)
	{
		RecoilCameraTimeline.PlayFromStart();

		if (RandomRangeFromRecoilCurveY.Num() == 2) RandomValueForCameraYRecoil = FMath::FRandRange(RandomRangeFromRecoilCurveY[0], RandomRangeFromRecoilCurveY[1]);
		bCanRecoilCamera = true;
	}
	else if ((PitchRecoilRangeArray.Num() == 2 && YawRecoilRangeArray.Num() == 2))
	{
		RandomRecoilYaw = FMath::FRandRange(YawRecoilRangeArray[0], YawRecoilRangeArray[1]);
		RandomRecoilPitch = FMath::FRandRange(PitchRecoilRangeArray[0], PitchRecoilRangeArray[1]);
		if (StatusOfGun == ADS)
		{
			RandomRecoilYaw /= DividerOfRecoilWhileADS;
			RandomRecoilPitch /= DividerOfRecoilWhileADS;
		}
	}
}

void AGun::UpRecoilCamera(float Delta)
{
	if (bCanRecoilCamera == false) return;

	if (bShouldUseCurveRecoil)
	{
		float ControlRotationPitch = (DistanceFromStart * 0.375) * TimeRecoilCameraElapsed / ((OriginalMagazineCapacity * RecoilAnimTimelineLength) + 0.2f);
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
	FVector Location = BaseSkeletalMesh->GetSocketLocation(TEXT("Bullet"));
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
	if (PitchBulletRecoilArray.Num() == 2 && YawBulletRecoilArray.Num() == 2 && (bIsAutomatic == false || bFirstBulletWithoutRecoil == false))
	{
		float NewPitchRotaton = FMath::FRandRange(PitchBulletRecoilArray[0], PitchBulletRecoilArray[1]);
		float NewYawRotation = FMath::FRandRange(YawBulletRecoilArray[0], YawBulletRecoilArray[1]);
		if (StatusOfGun == ADS)
		{
			NewPitchRotaton /= DividerOfBulletRecoilWhileADS;
			NewYawRotation /= DividerOfBulletRecoilWhileADS;
		}
		BulletRotation.Pitch += NewPitchRotaton;
		BulletRotation.Yaw += NewYawRotation;
	}
}
#pragma endregion

#pragma region ////////////////////////////////// RELOAD //////////////////////////////////////
bool AGun::CanReload()
{
	if (GetPointerToAmmoFromInventory() == false || MagazineCapacity == OriginalMagazineCapacity || GetWorldTimerManager().IsTimerActive(ReloadHandle)) return false;
	if (AmmunitionFromInventory->Item_Amount <= 0) return false;

	return true;
}

void AGun::WaitToReload()
{
	if (CanReload() == false) return;

	ReloadEffects();

	bCanShoot = false;
	ShootReleased();

	bIsReloading = true;
	MarinePawn->GetWeaponHandlerComponent()->ADSReleased();
	GetWorldTimerManager().SetTimer(ReloadHandle, this, &AGun::Reload, ReloadTime, false);
}

void AGun::ReloadEffects()
{
	if (ReloadSound) SpawnedReloadSound = UGameplayStatics::SpawnSoundAttached(ReloadSound, BaseSkeletalMesh);
	if (bCasingEjectionWhileReloading == true) DropCasing();

	BaseSkeletalMesh->PlayAnimation(WeaponReloadAnim.WeaponActionAnim, false);
	MarinePawn->GetArmsSkeletalMesh()->PlayAnimation(WeaponReloadAnim.ArmsActionAnim, false);
}

void AGun::Reload()
{
	if (GetPointerToAmmoFromInventory() == false) return;

	RemoveAmmunitionFromInventory();

	bIsReloading = false;
	bCanShoot = true;
	UpdateWeaponDataInHud(true);

	if (AmmunitionFromInventory->Item_Amount <= 0) MarinePawn->GetInventoryComponent()->Inventory_Items.Remove(SpawnedAmmoItemData.Item_Name);
	MarinePawn->UpdateAlbertosInventory();

	GetWorldTimerManager().ClearTimer(ReloadHandle);
	if (bReloadOneBullet) WaitToReload();
}

void AGun::CancelReload()
{
	GetWorldTimerManager().ClearTimer(ReloadHandle);

	if (SpawnedReloadSound) SpawnedReloadSound->ToggleActive();
	if (BaseSkeletalMesh->IsPlaying()) BaseSkeletalMesh->SetForceRefPose(true);

	SpawnedReloadSound = nullptr;
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
	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, BaseSkeletalMesh, NAME_None);
	if (ShootParticle) UGameplayStatics::SpawnEmitterAttached(ShootParticle, BaseSkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));

	if (bPlayShootAnimationAfterFire == true) return;

	PlayGunShootAnimation();

	if (bCasingEjectionWhileReloading == false) DropCasing();
}

void AGun::PlayGunShootAnimation()
{
	/*if (ShootWithNoBulletsAnimation && GetPointerToAmmoFromInventory() == true && MagazineCapacity == 1)
	{
		if (AmmunitionFromInventory->Item_Amount <= 0)
		{
			BaseSkeletalMesh->PlayAnimation(ShootWithNoBulletsAnimation, false);
			return;
		}
	}*/

	BaseSkeletalMesh->PlayAnimation(WeaponShootAnim.WeaponActionAnim, false);
	MarinePawn->GetArmsSkeletalMesh()->PlayAnimation(WeaponShootAnim.ArmsActionAnim, false);
}

void AGun::DropCasing()
{
	if (!DropBulletClass) return;

	FRotator DropBulletRotation = GetActorRotation();
	if (bShouldRandomizeRotationOfCasing)
	{
		DropBulletRotation.Yaw -= FMath::FRandRange(-10.f, 40.f);
		DropBulletRotation.Roll += FMath::FRandRange(-10.f, 10.f);
		DropBulletRotation.Pitch += FMath::FRandRange(-15.f, 15.f);
	}
	AActor* DropBullet = GetWorld()->SpawnActor<AActor>(DropBulletClass, BaseSkeletalMesh->GetSocketLocation(TEXT("BulletDrop")), DropBulletRotation);
	DropBullet->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}
#pragma endregion

#pragma region ///////////////////////////////// ADS //////////////////////////////////////////
void AGun::AimTheGun()
{
	/*if (StatusOfGun == HipFire) return;

	FVector Location = BaseSkeletalMesh->GetRelativeLocation();
	if (StatusOfGun == ADS)
	{
		BaseSkeletalMesh->SetRelativeLocation(FMath::Lerp(Location, RelativeLocationInPawnWhileADS, Delta * SpeedOfInterpADS));
		ChangeScopeResolution(StatusOfGun);
	}
	else
	{
		if (BaseSkeletalMesh->GetRelativeLocation().Equals(RelativeLocationInPawn, 1.5f))
		{
			StatusOfGun = HipFire;
			ChangeScopeResolution(StatusOfGun);
		}
	}*/
}
#pragma endregion

#pragma region ///////////////////////////////// HUD //////////////////////////////////////////
void AGun::SetHudWidget(UHUDWidget* NewHudWidget)
{
	if (NewHudWidget)
	{
		NewHudWidget->HideWeaponUI(false);
	}
	else if (HudWidget)
	{
		HudWidget->HideWeaponUI(true);
	}

	HudWidget = NewHudWidget;
}

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
void AGun::TakeGun(class AMarineCharacter* Player)
{
	if (IsValid(Player) == false)
		return;

	BaseSkeletalMesh->PlayAnimation(WeaponFirstTimeTakeAnim.WeaponActionAnim, false);
	Player->GetArmsSkeletalMesh()->PlayAnimation(WeaponFirstTimeTakeAnim.ArmsActionAnim, false);

	MarinePawn = Player;
	CopyOfFOV = MarinePawn->GetCamera()->FieldOfView;

	AddAmmoToInventory();
	MarinePawn->GetWeaponHandlerComponent()->SetCanChangeWeapon(false);
	MarinePawn->GetWeaponHandlerComponent()->SetGun(this);

	ActivateZoom(true);

	SetHudWidget(MarinePawn->GetHudWidget());
	UpdateWeaponDataInHud(true, true);

	MarinePawn->GetWeaponHandlerComponent()->SetCanChangeWeapon(true);
}
#pragma endregion

#pragma region ////////////////////////////////// DROP ////////////////////////////////////////
#pragma endregion

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
	if (MarinePawn->GetInventoryComponent() == nullptr) return;

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

void AGun::SetupFloatTimeline(FTimeline* TimelineToCreate, FName TimelineProgressFuncName, FName TimelineFinishedFuncName)
{
	FOnTimelineFloat TimelineFloatProgress;
	TimelineFloatProgress.BindUFunction(this, TimelineProgressFuncName);
	TimelineToCreate->AddInterpFloat(ShootFOVCurve, TimelineFloatProgress);

	FOnTimelineEventStatic TimelineCallbackFinished;
	TimelineCallbackFinished.BindUFunction(this, TimelineFinishedFuncName);
	TimelineToCreate->SetTimelineFinishedFunc(TimelineCallbackFinished);
}