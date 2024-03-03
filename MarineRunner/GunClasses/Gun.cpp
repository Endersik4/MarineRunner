// Copyright Adam Bartela.All Rights Reserved

#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Components/TimelineComponent.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/GunClasses/Scope.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/ArmsSwayComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Skeletal Mesh"));
	RootComponent = GunSkeletalMesh;

	GunSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));

	CameraRecoilPitchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Camera Recoil Pitch Timeline"));
	ShootRecoilTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Shoot Recoil Timeline"));

	Tags.Add(TEXT("Gun"));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	if (RecoilCameraCurveRandomRotation)
	{
		FOnTimelineFloat TimelineFloatProgress;
		TimelineFloatProgress.BindUFunction(this, "ShootRecoilTimelineProgress");
		ShootRecoilTimeline->AddInterpFloat(RecoilCameraCurveRandomRotation, TimelineFloatProgress);
	}

	if (bShouldUseCurveRecoil)
	{
		FOnTimelineFloat TimelineFloatProgress;
		TimelineFloatProgress.BindUFunction(this, "RecoilCameraTimelineCallback");
		CameraRecoilPitchTimeline->AddInterpFloat(RecoilCameraCurveY, TimelineFloatProgress);

		FOnTimelineEventStatic TimelineCallbackFinished;
		TimelineCallbackFinished.BindUFunction(this, "RecoilCameraTimelineFinishedCallback");
		CameraRecoilPitchTimeline->SetTimelineFinishedFunc(TimelineCallbackFinished);
	}

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

	UpRecoilCameraUsingCurve(DeltaTime);
	CameraInterpBackToInitialPosition(DeltaTime);
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
	if (CanShoot() == false) 
		return;

	if (bIsAutomatic)
	{
		// if the player has released the shoot button before ShootFinished(), the weapon will fire indefinitely, preventing the bug
		if (bShootButtonPressed == false) 
		{
			ShootReleased();
			return;
		}
		bConstantlyShoot = true;
	}

	if (bCanShoot == false )
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

	if (ShootCameraShake != NULL)
		PC->ClientStartCameraShake(ShootCameraShake);
	PlayRecoil();
	GetWorldTimerManager().SetTimer(PlayRecoilHandle, this, &AGun::ShootFinished, ShootTime, false);
}

void AGun::ShootReleased()
{
	bShootButtonPressed = false;
	if (bCanRecoilCamera == false) 
		return;

	GetWorldTimerManager().SetTimer(FirstBulletHandle, this, &AGun::ShouldFirstBulletGoStraight, FirstBulletWithoutRecoilTime, false);
	ResetVariablesForCameraRecoil();
	if (bShouldUseCurveRecoil) BackCameraToItsInitialRotation();
}

void AGun::ShootFinished()
{
	SetCanShoot();

	if (bConstantlyShoot || bShouldDelayShoot)
	{
		bShouldDelayShoot = false;
		bConstantlyShoot = false;
		Shoot();
	}
}
#pragma endregion

#pragma region ////////////////////////////////// RECOIL //////////////////////////////////////
void AGun::PlayRecoil()
{
	SetCameraRecoil(); //Recoil CAMERA
	ShootRecoilTimeline->PlayFromStart();
}
#pragma endregion

#pragma region //////////////////////////////// GUN RECOIL ////////////////////////////////////
void AGun::ShootRecoilTimelineProgress(float RecoilDirection)
{
	float ControlRotationPitch = RandomRecoilPitch * RecoilCameraCurveRandomRotation->GetFloatValue(ShootRecoilTimeline->GetPlaybackPosition());
	float ControlRotationYaw = RandomRecoilYaw * RecoilCameraCurveRandomRotation->GetFloatValue(ShootRecoilTimeline->GetPlaybackPosition());
		
	ControlRotationYaw *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	ControlRotationPitch *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	PC->AddYawInput(ControlRotationYaw);
	PC->AddPitchInput(-ControlRotationPitch);
}
#pragma endregion

#pragma region ////////////////////////////// CAMERA RECOIL ///////////////////////////////////
void AGun::RecoilCameraTimelineCallback(float ControlRotationY)
{
	//Randomize recoil a bit
	ControlRotationY += RandomValueForCameraYRecoil;

	if (StatusOfGun == ADS)
		ControlRotationY /= DividerOfRecoilWhileADS;

	ControlRotationY *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	PC->AddYawInput(ControlRotationY);
}

void AGun::SetCameraRecoil()
{
	if (bCanRecoilCamera == true) return;

	InitialCameraRotation = PC->GetControlRotation();

	if (bShouldUseCurveRecoil)
	{
		CameraRecoilPitchTimeline->PlayFromStart();
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

void AGun::UpRecoilCameraUsingCurve(float Delta)
{
	if (bCanRecoilCamera == false || bShouldUseCurveRecoil == false) 
		return;
	
	float ControlRotationPitch = (DistanceFromStart * 0.375) * Delta / ((OriginalMagazineCapacity * ShootTime) + 0.2f);
	if (StatusOfGun == ADS)
	{
		ControlRotationPitch /= DividerOfRecoilWhileADS;
	}
	ControlRotationPitch *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	PC->AddPitchInput(-ControlRotationPitch);
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
	if (IsValid(MarinePawn) == false) 
		return;

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

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(PC->GetControlRotation(), InitialCameraRotation, Delta * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), InitalCameraPositionSpeed);
	PC->SetControlRotation(NewRotation);
}

void AGun::ResetVariablesForCameraRecoil()
{
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	bCanRecoilCamera = false;
	bConstantlyShoot = false;

	if (bShouldUseCurveRecoil) 
		CameraRecoilPitchTimeline->Stop();
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
	if (bIsReloading == true)
		return false;

	FItemStruct* AmmoFromInventory = PlayerInventory->GetItemFromInventory(RowNameForAmmunitionItem);
	if (AmmoFromInventory == nullptr || MagazineCapacity == OriginalMagazineCapacity || GetWorldTimerManager().IsTimerActive(ReloadHandle))
	{
		bIsReloading = false;
		return false;
	}
	if (AmmoFromInventory->Item_Amount <= 0)
	{
		bIsReloading = false;
		return false;
	}

	if (bCanShoot == false)
		return false;

	return true;
}

void AGun::WaitToReload()
{
	if (CanReload() == false) 
		return;

	bCanShoot = false;
	ShootReleased();

	if (bReloadOneBullet)
		CurrentReloadType = GetCurrentReloadTypeAccordingToSituation();
	
	MarinePawn->GetWeaponHandlerComponent()->ADSReleased();

	const FWeaponAnimation& ReloadAnimToPlay = ReloadAnimAccordingToSituation();
	PlayGivenWeaponWithArmsAnimation(ReloadAnimToPlay);
	StartTimerToDropCasing(EWSC_WhileReloading);

	bIsReloading = true;

	GetWorldTimerManager().SetTimer(ReloadHandle, this, &AGun::Reload, ReloadAnimToPlay.ArmsActionAnim->GetPlayLength(), false);
}

const FWeaponAnimation& AGun::ReloadAnimAccordingToSituation()
{
	if (MagazineCapacity == 0)
		return WeaponReloadWithNoBulletsAnim;

	if (bReloadOneBullet == true)
	{
		if (CurrentReloadType == ERT_BeginEndReload)
			return WeaponReload_BeginEnd;

		if (CurrentReloadType == ERT_BeginReload)
			return WeaponReload_Begin;

		if (CurrentReloadType == ERT_EndReload)
			return WeaponReload_End;
	}

	return WeaponReloadAnim;
}

void AGun::Reload()
{
	FItemStruct* AmmoFromInventory = PlayerInventory->GetItemFromInventory(RowNameForAmmunitionItem);
	if (AmmoFromInventory == nullptr)
		return;

	RemoveAmmunitionFromInventory(AmmoFromInventory);
	if (AmmoFromInventory->Item_Amount <= 0)
		PlayerInventory->DeleteItemFromInventory(*AmmoFromInventory);

	bCanShoot = true;
	UpdateWeaponDataInHud(true);

	MarinePawn->UpdateAlbertosInventory();

	GetWorldTimerManager().ClearTimer(ReloadHandle);
	bIsReloading = false;

	if (bReloadOneBullet) 
		WaitToReload();
}

void AGun::CancelReload()
{
	GetWorldTimerManager().ClearTimer(ReloadHandle);
	GetWorldTimerManager().ClearTimer(DropCasingHandle);

	bCanShoot = true;
	bIsReloading = false;
	CurrentReloadType = ERT_EndReload;
}

void AGun::RemoveAmmunitionFromInventory(FItemStruct* AmmoFromInventory)
{
	if (bReloadOneBullet == true)
	{
		MagazineCapacity++;
		AmmoFromInventory->Item_Amount--;
		return;
	}

	int32 RestAmmo = OriginalMagazineCapacity - MagazineCapacity;
	if (AmmoFromInventory->Item_Amount < RestAmmo)
	{
		MagazineCapacity += AmmoFromInventory->Item_Amount;
		AmmoFromInventory->Item_Amount = 0;
	}
	else
	{
		AmmoFromInventory->Item_Amount -= RestAmmo;
		MagazineCapacity = OriginalMagazineCapacity;
	}
}

EReloadType AGun::GetCurrentReloadTypeAccordingToSituation()
{

	if (CurrentReloadType == ERT_EndReload && (MagazineCapacity == OriginalMagazineCapacity - 1 || PlayerInventory->GetItemFromInventory(RowNameForAmmunitionItem)->Item_Amount <= 1))
	{
		return ERT_BeginEndReload;
	}
	if (CurrentReloadType == ERT_EndReload || CurrentReloadType == ERT_BeginEndReload)
	{
		return ERT_BeginReload;
	}
	if ((CurrentReloadType == ERT_Reload || CurrentReloadType == ERT_BeginReload) && (MagazineCapacity == OriginalMagazineCapacity - 1 || PlayerInventory->GetItemFromInventory(RowNameForAmmunitionItem)->Item_Amount <= 1))
	{
		return ERT_EndReload;
	}

	return ERT_Reload;
}
#pragma endregion

#pragma region ////////////////////////////////// EFFECTS /////////////////////////////////////
void AGun::AddEffectsToShooting()
{
	if (ShootingSound)
	{
		UGameplayStatics::SpawnSoundAttached(ShootingSound, GunSkeletalMesh, NAME_None);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GunSkeletalMesh->GetComponentLocation(), 1.f, this);
	}
	if (ShootParticle) 
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, GunSkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));

	PlayGunShootAnimation();

	StartTimerToDropCasing(EWSC_WhileShooting);
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

void AGun::StartTimerToDropCasing(const EWhenSpawnCasing& CurrentSpawnCasingPeriod)
{
	if (bCasingEjection == false || WhenSpawnCasing != CurrentSpawnCasingPeriod)
		return;

	GetWorldTimerManager().SetTimer(DropCasingHandle, this, &AGun::DropCasing, SpawnCasingAfterTime, false);
}

void AGun::DropCasing()
{
	if (DropBulletClass == nullptr) 
		return;

	FRotator DropBulletRotation = GetActorRotation();
	if (bShouldRandomizeRotationOfCasing)
	{
		DropBulletRotation.Roll += FMath::FRandRange(RandomCasingRotation_Roll.GetLowerBoundValue(), RandomCasingRotation_Roll.GetUpperBoundValue());
		DropBulletRotation.Pitch += FMath::FRandRange(RandomCasingRotation_Pitch.GetLowerBoundValue(), RandomCasingRotation_Pitch.GetUpperBoundValue());
		DropBulletRotation.Yaw += FMath::FRandRange(RandomCasingRotation_Yaw.GetLowerBoundValue(), RandomCasingRotation_Yaw.GetUpperBoundValue());
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
	if (StatusOfGun == EStatusOfAimedGun::ADS)
		MarinePawn->GetArmsSwayComponent()->ResetArmsLocation();

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

	if (bChangeStoredAmmoText)
	{
		FItemStruct* AmmoFromInventory = PlayerInventory->GetItemFromInventory(RowNameForAmmunitionItem);
		HudWidget->SetAmmoText(AmmoFromInventory ? AmmoFromInventory->Item_Amount : 0, true);
	}
	if (bChangeWeaponImage)
	{
		HudWidget->SetWeaponImage(GunHUDTexture, bAmmoCounterBelowGunHUD);
	}
}
#pragma endregion

#pragma region ////////////////////////////////// TAKE ////////////////////////////////////////
void AGun::TakeGun(AMarineCharacter* Player, bool bWasTaken, int32 SavedMagazineCapacity)
{
	if (IsValid(Player) == false)
		return;

	MarinePawn = Player;
	bCanShoot = false;
	PlayerInventory = MarinePawn->GetInventoryComponent();
	MarinePawn->GetArmsSkeletalMesh()->SetForceRefPose(false);

	MarinePawn->GetWeaponInventoryComponent()->SetGunFromInventory(nullptr);

	MarinePawn->GetWeaponHandlerComponent()->HideCurrentHoldingGun();
	MarinePawn->GetWeaponHandlerComponent()->SetCanChangeWeapon(false);

	PlayGivenWeaponWithArmsAnimation(bWasTaken ? WeaponDrawAnim : WeaponFirstTimeTakeAnim);

	if (bWasTaken == false)
		AddAmmoToInventory();
	else
		MagazineCapacity = SavedMagazineCapacity;

	MarinePawn->GetWeaponHandlerComponent()->SetGun(this);
	MarinePawn->GetWeaponInventoryComponent()->AddNewWeaponToStorage(this);


	HudWidget = MarinePawn->GetHudWidget();
	HudWidget->ShowWeaponOnHud();
	UpdateWeaponDataInHud(true, true);

	FTimerHandle TakeGunHandle;
	GetWorld()->GetTimerManager().SetTimer(TakeGunHandle, this, &AGun::SetCanShoot, (bWasTaken ? WeaponDrawAnim : WeaponFirstTimeTakeAnim).ArmsActionAnim->GetPlayLength(), false);
}

void AGun::DrawGun()
{
	if (IsValid(MarinePawn) == false)
		return;

	SetActorHiddenInGame(false);

	CancelReload();

	SetActorTickEnabled(true);
	bCanShoot = false;
	MarinePawn->GetWeaponHandlerComponent()->SetGun(this);

	PlayGivenWeaponWithArmsAnimation(WeaponDrawAnim);

	if (DrawGunSound) 
		UGameplayStatics::PlaySound2D(GetWorld(), DrawGunSound);

	HudWidget->ShowWeaponOnHud();
	UpdateWeaponDataInHud(true, true);

	FTimerHandle DrawGunHandle;
	GetWorld()->GetTimerManager().SetTimer(DrawGunHandle, this, &AGun::SetCanShoot, WeaponDrawAnim.ArmsActionAnim->GetPlayLength(), false);
}

void AGun::PutAwayGun()
{
	if (IsValid(MarinePawn) == false)
		return;

	CancelReload();

	bCanShoot = false;

	MarinePawn->GetWeaponHandlerComponent()->SetGun(nullptr);

	PlayGivenWeaponWithArmsAnimation(WeaponHideAnim);
	ShootReleased();

	if (PutAwayGunSound) 
		UGameplayStatics::PlaySound2D(GetWorld(), PutAwayGunSound);

	HudWidget->ShowWeaponOnHud(false);

	FTimerHandle HideGunHandle;
	GetWorld()->GetTimerManager().SetTimer(HideGunHandle, this, &AGun::HideGun, WeaponHideAnim.ArmsActionAnim->GetPlayLength(), false);
}

void AGun::HideGun()
{
	CancelReload();
	bCanShoot = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	GunSkeletalMesh->Stop();

	MarinePawn->GetArmsSkeletalMesh()->Stop();
	GetWorld()->GetTimerManager().ClearTimer(PlayRecoilHandle);

	if (MarinePawn->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1 && bDropGun == true)
	{
		MarinePawn->GetArmsSkeletalMesh()->SetForceRefPose(true);
	}
	else 
		MarinePawn->GetWeaponHandlerComponent()->DrawNewGun();

	DropGun();
}

#pragma endregion

#pragma region ////////////////////////////////// DROP ////////////////////////////////////////
void AGun::DropGun()
{
	if (bDropGun == false)
		return;

	MarinePawn->GetWeaponInventoryComponent()->RemoveWeaponFromStorage(this);

	if (ItemToSpawnAfterDropGun == NULL)
		return;

	if (DropGunSound)
		UGameplayStatics::PlaySound2D(GetWorld(), DropGunSound);

	FVector LocationToSpawnItemGun = MarinePawn->GetCameraLocation() + MarinePawn->GetCamera()->GetForwardVector() * DistanceToDropGun;
	FTransform ItemGunTransform = FTransform(FRotator(0.f), LocationToSpawnItemGun);
	APickupItem* SpawnedGunItem = GetWorld()->SpawnActorDeferred<APickupItem>(ItemToSpawnAfterDropGun, ItemGunTransform);
	if (IsValid(SpawnedGunItem) == false)
		return;

	SpawnedGunItem->SetCurrentMagazineCapacity(MagazineCapacity);
	SpawnedGunItem->SetItemWasOnceTaken(true);
	SpawnedGunItem->SaveItemIfSpawnedRunTime();
	SpawnedGunItem->FinishSpawning(ItemGunTransform);

	FItemStruct* WeaponInformation = MarinePawn->GetInventoryComponent()->GetItemInformationFromDataTable(SpawnedGunItem->GetItemRowName());
	if (WeaponInformation)
		MarinePawn->GetInventoryComponent()->DeleteItemFromInventory(*WeaponInformation);
	MarinePawn->UpdateAlbertosInventory(true);

	Destroy();
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
		MarinePawn->GetArmsSkeletalMesh()->PlayAnimation(AnimToPlay.ArmsActionAnim, false);
	}
}

#pragma region /////////////////////////////// INVENTORY //////////////////////////////////////

void AGun::AddAmmoToInventory()
{
	if (PlayerInventory == nullptr)
		return;

	FItemStruct* AmmunitionFromInventory = PlayerInventory->GetItemFromInventory(RowNameForAmmunitionItem);

	if (AmmunitionFromInventory)
	{
		AmmunitionFromInventory->Item_Amount += StoredAmmo;
	}
	else
		PlayerInventory->AddNewItemToInventory(RowNameForAmmunitionItem, StoredAmmo);
}

#pragma endregion

