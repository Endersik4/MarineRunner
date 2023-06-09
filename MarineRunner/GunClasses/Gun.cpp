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
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Inventory/PickupItem.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BaseSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Skeletal Mesh"));
	RootComponent = BaseSkeletalMesh;

	BaseSkeletalMesh->SetSimulatePhysics(true);
	BaseSkeletalMesh->SetCollisionProfileName(TEXT("GunCollision"));
	BaseSkeletalMesh->SetNotifyRigidBodyCollision(true); 
	BaseSkeletalMesh->bRenderCustomDepth = true; //Its for outline material 

	Tags.Add(TEXT("Gun"));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	BaseSkeletalMesh->OnComponentHit.AddDynamic(this, &AGun::OnHit);

	SpawnAmmunitionForVariables();

	RecoilAnimTimeline = SetupTimeline(RecoilAnimTimeline, RecoilAnimCurveLocationX, FName("RecoilAnimTimeline"), FName("RecoilAnimTimelineDirection"), RecoilAnimTimelineLength, FName("RecoilAnimTimelineCallback"), FName("RecoilAnimTimelineFinishedCallback"));
	if (bShouldUseCurveRecoil) RecoilCameraTimeline = SetupTimeline(RecoilCameraTimeline, RecoilCameraCurveY, FName("RecoilCameraTimeline"), FName("RecoilCameraTimelineDirection"), RecoilCameraTimelineLength, FName("RecoilCameraTimelineCallback"), FName("RecoilCameraTimelineFinishedCallback"));
	
	CopyOfMagazineCapacity = MagazineCapacity;
	PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GunSway();
	AimTheGun(DeltaTime);
	UpRecoilCamera();
	InterpBackToInitialPosition();
}

#pragma region //////////////////////////////////// SHOOT /////////////////////////////////////
bool AGun::CanShoot()
{
	if (BulletClass == NULL || bIsGrabbingEnded == false) return false;
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
	SetWeaponInHud();

	bCanShoot = false;
	bCanSway = false;
	bCanDropTheGun = false;

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
	//Recoil Things
	SetCameraRecoil(); //Recoil CAMERA
	Playtimeline(RecoilAnimTimeline); //Recoil GUN 
}
#pragma endregion

#pragma region //////////////////////////////// GUN RECOIL ////////////////////////////////////
void AGun::RecoilAnimTimelineCallback(float RecoilDirection)
{
	if (RecoilAnimCurvePitch)
	{
		float RecoilPitch = RecoilAnimCurvePitch->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		if (StatusOfGun == ADS) RecoilPitch /= DividerOfRecoilWhileADS;

		FRotator GunMeshRotation = BaseSkeletalMesh->GetRelativeRotation();
		GunMeshRotation.Pitch = RecoilPitch * MultiplierOfRecoilCurvePitch;

		BaseSkeletalMesh->SetRelativeRotation(GunMeshRotation);
	}

	if (RecoilAnimCurveScale)
	{
		float RecoilScale = RecoilAnimCurveScale->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		//if (StatusOfGun == ADS) RecoilScale /= DividerOfRecoilWhileADS;
		BaseSkeletalMesh->SetRelativeScale3D(FVector(RecoilScale));
	}

	if (RecoilCameraCurveRandomRotation)
	{
		float ControlRotationPitch = RandomRecoilPitch * RecoilCameraCurveRandomRotation->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		float ControlRotationYaw = RandomRecoilYaw * RecoilCameraCurveRandomRotation->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		PC->AddYawInput(ControlRotationYaw);
		PC->AddPitchInput(-ControlRotationPitch);
	}

	if (ShootFOVCurve)
	{
		float NewFOV = ShootFOVCurve->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		MarinePawn->GetCamera()->FieldOfView += NewFOV;
	}
	
	FVector GunMeshLocation = BaseSkeletalMesh->GetRelativeLocation();
	if (StatusOfGun == ADS) RecoilDirection /= DividerOfRecoilWhileADS;
	GunMeshLocation.X = GunMeshLocation.X - RecoilDirection;

	BaseSkeletalMesh->SetRelativeLocation(GunMeshLocation);
}

void AGun::RecoilAnimTimelineFinishedCallback()
{
	bCanSway = true;
	if (bPlayShootAnimationAfterFire)
	{
		if (AfterShootSound) UGameplayStatics::SpawnSoundAttached(AfterShootSound, BaseSkeletalMesh, NAME_None);
		BaseSkeletalMesh->SetForceRefPose(false);
		if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);
		FTimerHandle DropCasingHandle;
		GetWorldTimerManager().SetTimer(DropCasingHandle, this, &AGun::DropCasing, 0.2f, false);

		FTimerHandle PlayAfterFireHandle;
		GetWorldTimerManager().SetTimer(PlayAfterFireHandle, this, &AGun::SetCanShoot, ShootAnimation->GetPlayLength(), false);
	}
	else SetCanShoot();

	if (StatusOfGun == HipFire) bCanDropTheGun = true;

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
		Playtimeline(RecoilCameraTimeline); //Use Curve Recoil for Camera moving

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

void AGun::UpRecoilCamera()
{
	if (bCanRecoilCamera == false) return;
	
	if (bShouldUseCurveRecoil)
	{
		float ControlRotationPitch = (DistanceFromStart * 0.375) * TimeRecoilCameraElapsed / ((CopyOfMagazineCapacity * RecoilAnimTimelineLength) + 0.2f);
		PC->AddPitchInput(-ControlRotationPitch);
	}
	TimeRecoilCameraElapsed = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
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

	bShouldInterpBack = true;
}

void AGun::InterpBackToInitialPosition()
{
	if (!MarinePawn) return;
	
	if (bShouldInterpBack == false) return;

	if (PC->MouseXValue > 0.05f || PC->MouseXValue < -0.05f || PC->MouseYValue > 0.05f || PC->MouseYValue < -0.05f)
	{
		bShouldInterpBack = false;
		return;
	}

	if (PC->GetControlRotation().Equals(InitialCameraRotation, 0.05f))
	{
		bShouldInterpBack = false;
		return;
	}
	
	FRotator NewRotation = UKismetMathLibrary::RInterpTo(PC->GetControlRotation(), InitialCameraRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), InitalCameraPositionSpeed);

	PC->SetControlRotation(NewRotation);
}

void AGun::ResetVariablesForCameraRecoil()
{
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	bCanRecoilCamera = false;
	TimeRecoilCameraElapsed = 0.f;
	bConstantlyShoot = false;
	if (bShouldUseCurveRecoil) RecoilCameraTimeline->Stop();
}
#pragma endregion
 
#pragma region ////////////////////////////////// BULLET //////////////////////////////////////
void AGun::SpawnBullet()
{
	//Proper Location and Rotation for Bullet
	FVector Location = BaseSkeletalMesh->GetSocketLocation(TEXT("Bullet"));
	BulletRotation = GetActorRotation();

	//Bullet will randomly "go" to other directions 
	if (PitchBulletRecoilArray.Num() == 2 && YawBulletRecoilArray.Num() == 2 && (bIsAutomatic == false ||bFirstBulletWithoutRecoil == false))
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
	bFirstBulletWithoutRecoil = false;

	ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, Location, BulletRotation);

	float BulletDamage = (bManyBulletAtOnce == false ? Damage : Damage / HowManyBulletsToSpawn);
	float BulletImpulseForce = (bManyBulletAtOnce == false ? AmmoImpulseForce : AmmoImpulseForce / HowManyBulletsToSpawn);
	SpawnedBullet->SetBulletVariables(BulletDamage, AmmoSpeed, AmmoDistance, AmmoFallingDown, BulletImpulseForce);
	SpawnedBullet->ImpulseOnBullet(bShouldUseImpulseOnBullet);

	if (bCanBulletGoThrough == true) SpawnedBullet->SetBulletGoThroughVariables(true, DamageReduceAfterObject, ImpulseReduceAfterObject, MaxObjectsForBulletToGoThrough);
	if (bRadialImpulse) SpawnedBullet->RadialImpulse(RadialSphereRadius, bDrawRadialSphere);
	if (bShouldCameraShakeAfterHit) SpawnedBullet->SetCameraShake(CameraShakeAfterBulletHit);
}
#pragma endregion

#pragma region ////////////////////////////////// RELOAD //////////////////////////////////////
void AGun::WaitToReload()
{
	if (GetPointerToAmmoFromInventory() == false || MagazineCapacity == CopyOfMagazineCapacity || GetWorldTimerManager().IsTimerActive(ReloadHandle)) return;
	if (ItemFromInventory->Item_Amount <= 0) return;

	if (ReloadSound) SpawnedReloadSound = UGameplayStatics::SpawnSoundAttached(ReloadSound, BaseSkeletalMesh);
	if (bCasingEjectionWhileReloading == true) DropCasing();
	BaseSkeletalMesh->SetForceRefPose(false);
	if (ReloadAnimation) BaseSkeletalMesh->PlayAnimation(ReloadAnimation, false);

	bCanShoot = false;
	ShootReleased();
	bIsReloading = true;
	MarinePawn->CallADSReleased();
	GetWorldTimerManager().SetTimer(ReloadHandle, this, &AGun::Reload, ReloadTime, false);
}

void AGun::Reload()
{
	if (GetPointerToAmmoFromInventory() == false) return;

	if (bReloadOneBullet == true)
	{
		MagazineCapacity++;
		ItemFromInventory->Item_Amount--;
	}
	else
	{
		int32 RestAmmo = CopyOfMagazineCapacity - MagazineCapacity;
		if (ItemFromInventory->Item_Amount < RestAmmo)
		{
			MagazineCapacity += ItemFromInventory->Item_Amount;
			ItemFromInventory->Item_Amount = 0;
		}
		else
		{
			ItemFromInventory->Item_Amount -= RestAmmo;
			MagazineCapacity = CopyOfMagazineCapacity;
		}
	}

	bIsReloading = false;
	bCanShoot = true;
	SetWeaponInHud(true);
	GetWorldTimerManager().ClearTimer(ReloadHandle);
	if (bReloadOneBullet) WaitToReload();
}

void  AGun::CancelReload()
{
	GetWorldTimerManager().ClearTimer(ReloadHandle);

	if (SpawnedReloadSound) SpawnedReloadSound->ToggleActive();
	if (BaseSkeletalMesh->IsPlaying()) BaseSkeletalMesh->SetForceRefPose(true);

	SpawnedReloadSound = nullptr;
	bCanShoot = true;
	bIsReloading = false;
}
#pragma endregion

#pragma region ////////////////////////////////// EFFECTS /////////////////////////////////////
void AGun::AddEffectsToShooting()
{
	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, BaseSkeletalMesh, NAME_None);
	if (ShootParticle) UGameplayStatics::SpawnEmitterAttached(ShootParticle, BaseSkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));

	if (bPlayShootAnimationAfterFire == true) return;

	BaseSkeletalMesh->SetForceRefPose(false);
	if (GetPointerToAmmoFromInventory() && MagazineCapacity == 1 && ShootWithNoBulletsAnimation)
	{
		if (ItemFromInventory->Item_Amount <= 0) BaseSkeletalMesh->PlayAnimation(ShootWithNoBulletsAnimation, false);
	}
	else if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);

	if (bCasingEjectionWhileReloading == false) DropCasing();
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

void AGun::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HitGroundSound) return;

	if (SpawnedHitGroundSound && HitActor)
	{
		if (SpawnedHitGroundSound->IsActive() && HitActor == OtherActor) return;
	}

	SpawnedHitGroundSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitGroundSound, GetActorLocation());
	HitActor = OtherActor;
}

#pragma region //////////////////////////////// GUN SWAY //////////////////////////////////////
void AGun::GunSway()
{
	if (MarinePawn == nullptr || bCanGunSwayTick == false || bCanSway == false) return;
	
	float Delta = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	//Preparing variables
	float LookUp = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputAxisValue("LookUp");
	float LookRight = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputAxisValue("LookRight");
	float Forward = MarinePawn->GetInputAxisValue("Forward");
	float Right = MarinePawn->GetInputAxisValue("Right");
	
	//Rotation Sway when The Player moves the mouse
	float InterpLookUp = FMath::FInterpTo(GunRotationSway.Pitch, UKismetMathLibrary::MapRangeClamped(LookUp, -1, 1, RotationSwayPitchRangeBack, RotationSwayPitchRangeUp), Delta, SpeedOfSwayPitch);
	float InterpLookRight = FMath::FInterpTo(GunRotationSway.Yaw, UKismetMathLibrary::MapRangeClamped(LookRight, -1, 1, RotationSwayYawRangeBack, RotationSwayYawRangeUp), Delta, SpeedOfSwayYaw);

	if (StatusOfGun == ADS)
	{
		InterpLookRight /= DividerOfGunSwayADS;
		InterpLookUp /= DividerOfGunSwayADS;
		GunRotationSway = FRotator(InterpLookUp, InterpLookRight, 0.f);
		BaseSkeletalMesh->SetRelativeRotation(GunRotationSway);
		return;
	}
	
	//Location Sway when The Player moves around
	FVector GunLocationInterp = BaseSkeletalMesh->GetRelativeLocation(); 
	float InterpForward = FMath::InterpEaseInOut(GunLocationInterp.X, UKismetMathLibrary::MapRangeClamped(Forward, 1, -1, RelativeLocationInPawn.X + LocationSwayXRangeBack, 
		RelativeLocationInPawn.X + LocationSwayXRangeUp), (Delta * SpeedOfSwayX), 1.f);
	float InterpRight = FMath::InterpEaseInOut(GunLocationInterp.Y, UKismetMathLibrary::MapRangeClamped(Right, -1, 1, RelativeLocationInPawn.Y + LocationSwayYRangeBack,
		RelativeLocationInPawn.Y + LocationSwayYRangeUp), (Delta * SpeedOfSwayY), 1.f);

	GunRotationSway = FRotator(InterpLookUp, InterpLookRight, 0.f);
	GunLocationInterp = FVector(InterpForward, InterpRight, GunLocationInterp.Z);
	BaseSkeletalMesh->SetRelativeLocation(GunLocationInterp);
	BaseSkeletalMesh->SetRelativeRotation(GunRotationSway);

	//If Player doing nothing then Gun goes back to its original position
	if (MarinePawn->GetInputAxisValue("Forward") == 0 && MarinePawn->GetInputAxisValue("Right") == 0)
	{
		FVector BackToOriginalPosition = FMath::VInterpTo(BaseSkeletalMesh->GetRelativeLocation(), RelativeLocationInPawn, Delta, 1.f);
		BaseSkeletalMesh->SetRelativeLocation(BackToOriginalPosition);
	}
}

void AGun::GunSwayWhileMoving()
{
	//Using Lemniscate Of Bernoulli to sway gun while moving 
	if (bCanSway == false || MarinePawn == nullptr) return;
	if (!MarinePawn->GetIsWallrunning() && MarinePawn->GetInputAxisValue("Forward") == 0 && MarinePawn->GetInputAxisValue("Right") == 0) return;

	float SpeedOfLemniscate = GetWorld()->GetTimeSeconds() * SpeedOfSwayWhileMoving;

	float Angle = 2 / (9.f - FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate * 2)));
	float LocationY = Angle * FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate)); //SineWave
	float LocationZ = (FMath::Sin(FMath::DegreesToRadians(SpeedOfLemniscate * 2)) * Angle) / 2; //Angle Offset

	if (StatusOfGun == ADS)
	{
		Angle /= DividerOfGunSwayMovingADS;
		LocationY /= DividerOfGunSwayMovingADS;
		LocationZ /= DividerOfGunSwayMovingADS;
	}
	
	FVector GunLemniscateLocation = BaseSkeletalMesh->GetRelativeLocation();
	GunLemniscateLocation.Y += (LocationY * MultiplierOfLocationYSwayWhileMoving);
	GunLemniscateLocation.Z += (LocationZ * MultiplierOfLocationZSwayWhileMoving);

	BaseSkeletalMesh->SetRelativeLocation(GunLemniscateLocation);
	
}

void AGun::SetGunSwayWhileMovingTimer(bool bShouldClear)
{
	if (bShouldClear)
	{
		GetWorldTimerManager().ClearTimer(GunSwayWhileMovingHandle);
	}
	else
	{
		GetWorldTimerManager().SetTimer(GunSwayWhileMovingHandle, this, &AGun::GunSwayWhileMoving, 0.01f, true);
	}
}
#pragma endregion

#pragma region ///////////////////////////////// ADS //////////////////////////////////////////
void AGun::AimTheGun(float Delta)
{
	if (StatusOfGun == HipFire) return;

	FVector Location = BaseSkeletalMesh->GetRelativeLocation();
	if (StatusOfGun == ADS)
	{
		BaseSkeletalMesh->SetRelativeLocation(FMath::Lerp(Location, RelativeLocationInPawnWhileADS, Delta * SpeedOfInterpADS));
		bCanDropTheGun = false;
	}
	else
	{
		BaseSkeletalMesh->SetRelativeLocation(FMath::Lerp(Location, RelativeLocationInPawn, Delta * SpeedOfBackToHipFire));
		if (BaseSkeletalMesh->GetRelativeLocation().Equals(RelativeLocationInPawn, 1.5f))
		{
			bCanDropTheGun = true;
			StatusOfGun = HipFire;
		}
	}
}
#pragma endregion

#pragma region ///////////////////////////////// HUD //////////////////////////////////////////
void AGun::SetHudWidget(UHUDWidget* NewHudWidget)
{
	if (NewHudWidget)
	{
		NewHudWidget->HideWeaponThings(false);
	}
	else if (HudWidget)
	{
		HudWidget->HideWeaponThings(true);
	}

	HudWidget = NewHudWidget;
}

void AGun::SetWeaponInHud(bool bChangeStoredAmmoText, bool bChangeWeaponImage)
{
	if (!HudWidget) return;

	HudWidget->SetAmmoText(MagazineCapacity);

	if (bChangeStoredAmmoText && GetPointerToAmmoFromInventory()) HudWidget->SetAmmoText(ItemFromInventory->Item_Amount, true);
	if (bChangeWeaponImage)
	{
		HudWidget->SetWeaponImage(GunHUDTexture, bAmmoCounterBelowGunHUD);
	}
}
#pragma endregion

#pragma region ////////////////////////////////// TAKE ////////////////////////////////////////
void AGun::TakeItem(AMarineCharacter* MarineCharacter, bool& bIsItWeapon)
{
	bool bIsTooManyItems = MarineCharacter->GetWeaponInventoryComponent()->GetWeaponsStorageAmount() >= MarineCharacter->GetWeaponInventoryComponent()->GetMaxAmount();
	if (bIsTooManyItems)
	{
		bIsItWeapon = false;
		return;
	}

	bIsGrabbingEnded = false;
	bIsItWeapon = true;
	MarinePawn = MarineCharacter;

	AddAmmoToInventory();

	EquipWeapon();

	MarinePawn->SetCanChangeWeapon(false);
	MarinePawn->HideGunAndAddTheNewOne(this);
	MarinePawn->SetGun(this);
}

void AGun::EquipWeapon(bool bShouldPlaySound, bool bIsThisCurrentGun)
{
	BaseSkeletalMesh->SetSimulatePhysics(false);
	BaseSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseSkeletalMesh->SetRenderCustomDepth(false);

	if (bShouldPlaySound && PickUpSound) UGameplayStatics::SpawnSoundAttached(PickUpSound, BaseSkeletalMesh);

	SetHudWidget(MarinePawn->GetHudWidget());
	if (bIsThisCurrentGun)
	{
		//Changing Weapons things In HUD to the correct ones
		SetWeaponInHud(true, true);
	}

	AttachToComponent(MarinePawn->GetCamera(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

bool AGun::ItemLocationWhenGrabbed(float SpeedOfItem)
{
	if (bIsGrabbingEnded == true) return true;

	FVector BaseSkeletalMeshRelativeLocation = BaseSkeletalMesh->GetRelativeLocation();
	FVector Location = FMath::InterpExpoOut(BaseSkeletalMeshRelativeLocation, RelativeLocationInPawn, SpeedOfItem);
	SetActorRelativeLocation(Location);

	FRotator BaseSkeletalMeshRelativeRotation = BaseSkeletalMesh->GetRelativeRotation();
	FRotator Rotation = FMath::InterpExpoOut(BaseSkeletalMeshRelativeRotation, FRotator(0.f), SpeedOfItem);
	SetActorRelativeRotation(Rotation);

	return IsGunAtTheWeaponLocation();
}

bool AGun::IsGunAtTheWeaponLocation()
{
	bIsGrabbingEnded = BaseSkeletalMesh->GetRelativeLocation().Equals(RelativeLocationInPawn, 0.2f);
	if (!bIsGrabbingEnded) return false;

	SetGunSwayWhileMovingTimer();
	SetCanGunSwayTick(true);
	SetActorRelativeLocation(RelativeLocationInPawn);

	//MarinePawn->SetGun(this);
	MarinePawn->SetCanChangeWeapon(true);
	return true;
}
#pragma endregion

#pragma region ////////////////////////////////// DROP ////////////////////////////////////////
AActor* AGun::DropItem()
{
	if (bIsGrabbingEnded == false) return this;
	if (this != MarinePawn->GetGun()) return this;

	if (bCanDropTheGun == false) return this;

	MarinePawn->GetWeaponInventoryComponent()->RemoveWeaponFromStorage(this);
	int32 AmountOfWeapons = MarinePawn->GetWeaponInventoryComponent()->GetWeaponsStorageAmount();

	AActor* GunFromStorage = ChangeToAnotherWeapon(AmountOfWeapons);

	DropTheGun();
	return GunFromStorage;
}

void AGun::DropTheGun()
{
	if (!MarinePawn) return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	BaseSkeletalMesh->SetSimulatePhysics(true);
	BaseSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseSkeletalMesh->SetRenderCustomDepth(true);

	SetGunSwayWhileMovingTimer(true);
	bCanGunSwayTick = false;
	HudWidget = nullptr;
	if (bIsReloading) CancelReload();

	FVector DropImpulse = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetRootComponent()->GetForwardVector() * 10 * DropImpulseDistance;
	BaseSkeletalMesh->AddImpulse(DropImpulse);
	MarinePawn = nullptr;
}
#pragma endregion

#pragma region /////////////////////////////// INVENTORY //////////////////////////////////////
void AGun::SpawnAmmunitionForVariables()
{
	APickupItem* SpawnedAmmunition = GetWorld()->SpawnActor<APickupItem>(AmmunitionItemClass, FVector(0.f), FRotator(0.f));
	if (SpawnedAmmunition == nullptr) return;

	AmmoItem = SpawnedAmmunition->GetItemSettings();
	AmmoItem.Item_Amount += StoredAmmo;
	SpawnedAmmunition->Destroy();
}

void AGun::AddAmmoToInventory()
{
	if (bDidTakeThisWeapon == true || MarinePawn->GetInventoryComponent() == nullptr) return;

	if (GetPointerToAmmoFromInventory())
	{
		ItemFromInventory->Item_Amount += StoredAmmo;
	}
	else
	{
		MarinePawn->GetInventoryComponent()->Inventory_Items.Add(AmmoItem.Item_Name, AmmoItem);
		ItemFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(AmmoItem.Item_Name);
	}
	
	bDidTakeThisWeapon = true;
}

bool AGun::GetPointerToAmmoFromInventory()
{
	ItemFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(AmmoItem.Item_Name);
	return ItemFromInventory ? true : false;
}

AActor* AGun::ChangeToAnotherWeapon(int32 AmountOfWeapons)
{
	AGun* GunFromStorage;
	if (AmountOfWeapons > 0)
	{
		GunFromStorage = MarinePawn->GetWeaponInventoryComponent()->GetWeaponFromStorage(AmountOfWeapons, nullptr);
		MarinePawn->SetGun(GunFromStorage);
	}
	else
	{
		MarinePawn->GetHudWidget()->HideWeaponThings(true);
		GunFromStorage = nullptr;
		MarinePawn->SetGun(nullptr);
	}
	return GunFromStorage;
}
#pragma endregion

#pragma region /////////////////////////////// TIMELINES //////////////////////////////////////
UTimelineComponent* AGun::SetupTimeline(UTimelineComponent* TimeLineComp, UCurveFloat* MostImportantCurve, FName TimeLineName, FName TimeLineDirection, float TimeLineLength,
	FName TimelineCallbackFunction, FName TimelineFinishedFunction)
{
	if (!MostImportantCurve) return nullptr;

	FOnTimelineFloat onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	TimeLineComp = NewObject<UTimelineComponent>(this, TimeLineName); 
	TimeLineComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	this->BlueprintCreatedComponents.Add(TimeLineComp); 

	TimeLineComp->SetPropertySetObject(this); 
	TimeLineComp->SetDirectionPropertyName(TimeLineDirection); 
	TimeLineComp->SetIgnoreTimeDilation(true);

	TimeLineComp->SetLooping(false);
	TimeLineComp->SetTimelineLength(TimeLineLength); 
	TimeLineComp->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength); 

	onTimelineCallback.BindUFunction(this, TimelineCallbackFunction); 
	onTimelineFinishedCallback.BindUFunction(this, TimelineFinishedFunction); 
	TimeLineComp->AddInterpFloat(MostImportantCurve, onTimelineCallback); 
	TimeLineComp->SetTimelineFinishedFunc(onTimelineFinishedCallback);

	TimeLineComp->RegisterComponent();
	return TimeLineComp;
}

void AGun::Playtimeline(UTimelineComponent* TimeLineComp)
{
	if (TimeLineComp)
	{
		TimeLineComp->PlayFromStart();
	}
}
#pragma endregion