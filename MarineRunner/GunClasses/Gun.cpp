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
#include "MarineRunner/Widgets/HUDWidget.h"

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

	Tags.Add(TEXT("Gun"));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	BaseSkeletalMesh->OnComponentHit.AddDynamic(this, &AGun::OnHit);

	RecoilAnimTimeline = SetupTimeline(RecoilAnimTimeline, RecoilAnimCurveLocationX, FName("RecoilAnimTimeline"), FName("RecoilAnimTimelineDirection"), RecoilAnimTimelineLength, FName("RecoilAnimTimelineCallback"), FName("RecoilAnimTimelineFinishedCallback"));
	if (bShouldUseCurveRecoil) RecoilCameraTimeline = SetupTimeline(RecoilCameraTimeline, RecoilCameraCurveY, FName("RecoilCameraTimeline"), FName("RecoilCameraTimelineDirection"), RecoilCameraTimelineLength, FName("RecoilCameraTimelineCallback"), FName("RecoilCameraTimelineFinishedCallback"));
	
	CopyOfMagazineCapacity = MagazineCapacity;
	PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimTheGun(DeltaTime);
	GunSway();
	UpRecoilCamera();
	InterpBackToInitialPosition();
}

void AGun::Shoot()
{
	if (BulletClass == NULL) return;
	if (bReloadOneBullet && bIsReloading && MagazineCapacity > 0) CancelReload();
	else if (bIsReloading) return;

	if (bCanShoot == false)
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
	if (bManyBulletAtOnce) for (int i = 0; i != 10; i++) SpawnBullet();
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

void AGun::AddEffectsToShooting()
{
	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, BaseSkeletalMesh, NAME_None);
	if (ShootParticle) UGameplayStatics::SpawnEmitterAttached(ShootParticle, BaseSkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	
	if (bPlayShootAnimationAfterFire == true) return;

	if (StoredAmmo <= 0 && MagazineCapacity == 1 && ShootWithNoBulletsAnimation)
	{
		BaseSkeletalMesh->PlayAnimation(ShootWithNoBulletsAnimation, false);
	}
	else if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);

	DropCasing();
}

void AGun::DropCasing()
{
	if (!DropBulletClass) return;

	FRotator DropBulletRotation = GetActorRotation();
	DropBulletRotation.Yaw -= FMath::FRandRange(-10.f, 40.f);
	DropBulletRotation.Roll += FMath::FRandRange(-10.f, 10.f);
	DropBulletRotation.Pitch += FMath::FRandRange(-15.f, 15.f);
	AActor* DropBullet = GetWorld()->SpawnActor<AActor>(DropBulletClass, BaseSkeletalMesh->GetSocketLocation(TEXT("BulletDrop")), DropBulletRotation);
	DropBullet->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, TEXT("BulletDrop"));
}

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
		if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);
		FTimerHandle DropCasingHandle;
		GetWorldTimerManager().SetTimer(DropCasingHandle, this, &AGun::DropCasing, 0.2f, false);

		FTimerHandle PlayAfterFireHandle;
		GetWorldTimerManager().SetTimer(PlayAfterFireHandle, this, &AGun::SetCanShoot, ShootAnimation->GetPlayLength(), false);
	}
	else bCanShoot = true;

	if (StatusOfGun == HipFire) bCanDropTheGun = true;

	if (bShouldDelayShoot || bConstantlyShoot)
	{
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AGun::Shoot, 0.015f);
		if (bShouldDelayShoot) bShouldDelayShoot = false;
	}
}

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
	SpawnedBullet->SetBulletVariables(BulletDamage, AmmoSpeed, AmmoDistance, AmmoFallingDown, AmmoImpulseForce);
	SpawnedBullet->ImpulseOnBullet();
}

void AGun::ResetVariablesForCameraRecoil()
{
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	bCanRecoilCamera = false;
	TimeRecoilCameraElapsed = 0.f;
	bConstantlyShoot = false;
	if (bShouldUseCurveRecoil) RecoilCameraTimeline->Stop();
}

void AGun::WaitToReload()
{
	if (StoredAmmo <= 0 || MagazineCapacity == CopyOfMagazineCapacity || GetWorldTimerManager().IsTimerActive(ReloadHandle)) return;
	if (ReloadSound) SpawnedReloadSound = UGameplayStatics::SpawnSoundAttached(ReloadSound, BaseSkeletalMesh);

	bCanShoot = false;
	ShootReleased();
	bIsReloading = true;
	GetWorldTimerManager().SetTimer(ReloadHandle, this, &AGun::Reload, ReloadTime, false);
}

void AGun::Reload()
{
	if (bReloadOneBullet == true)
	{
		MagazineCapacity++;
		StoredAmmo--;
	}
	else
	{
		int32 RestAmmo = CopyOfMagazineCapacity - MagazineCapacity;
		if (StoredAmmo < RestAmmo)
		{
			MagazineCapacity += StoredAmmo;
			StoredAmmo = 0;
		}
		else
		{
			StoredAmmo -= RestAmmo;
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
	if (!SpawnedReloadSound) return;

	SpawnedReloadSound->ToggleActive();
	SpawnedReloadSound = nullptr;
	bCanShoot = true;
	bIsReloading = false;
}

void AGun::EquipWeapon(class AMarineCharacter* Marine, bool bShouldPlaySound, bool bIsThisCurrentGun)
{
	BaseSkeletalMesh->SetSimulatePhysics(false);
	BaseSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarinePawn = Marine;

	if (bShouldPlaySound && PickUpSound) UGameplayStatics::SpawnSoundAttached(PickUpSound, BaseSkeletalMesh);

	SetHudWidget(Marine->GetHudWidget());
	if (bIsThisCurrentGun)
	{
		//Changing Weapons things In HUD to the correct ones
		SetWeaponInHud(true, true);
	}

	AttachToComponent(Marine->GetCamera(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

void AGun::DropTheGun()
{
	if (!MarinePawn) return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	BaseSkeletalMesh->SetSimulatePhysics(true);
	BaseSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SetGunSwayWhileMovingTimer(true);
	bCanGunSwayTick = false;
	HudWidget = nullptr;
	if (bIsReloading) CancelReload();

	FVector DropImpulse = MarinePawn->GetCamera()->GetForwardVector() * 10 * DropImpulseDistance;
	BaseSkeletalMesh->AddImpulse(DropImpulse);
	MarinePawn = nullptr;
}

void AGun::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HitGroundSound || OtherActor->ActorHasTag("Gun")) return;

	if (SpawnedHitGroundSound && HitActor)
	{
		if (SpawnedHitGroundSound->IsActive() && HitActor == OtherActor) return;
	}

	SpawnedHitGroundSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitGroundSound, GetActorLocation());
	HitActor = OtherActor;
}

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
		InterpLookRight /= 1.5f;
		InterpLookUp /= 1.5f;
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
		Angle /= 2;
		LocationY /= 2;
		LocationZ /= 2;
	}
	
	FVector GunLemniscateLocation = BaseSkeletalMesh->GetRelativeLocation();
	GunLemniscateLocation.Y += (LocationY * MultiplierOfLocationYSwayWhileMoving);
	GunLemniscateLocation.Z += (LocationZ * MultiplierOfLocationZSwayWhileMoving);

	BaseSkeletalMesh->SetRelativeLocation(GunLemniscateLocation);
	
}

void AGun::AimTheGun(float Delta)
{
	if (StatusOfGun == HipFire) return;

	FVector Location = BaseSkeletalMesh->GetRelativeLocation();
	if (StatusOfGun == ADS)
	{
		FRotator Rotation = BaseSkeletalMesh->GetRelativeRotation();
		BaseSkeletalMesh->SetRelativeLocation(FMath::Lerp(Location, RelativeLocationInPawnWhileADS, Delta * SpeedOfInterpADS));
		BaseSkeletalMesh->SetRelativeRotation(FMath::RInterpTo(Rotation, RelativeRotationInPawn, Delta, SpeedOfInterpADS));
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
	if (bChangeStoredAmmoText) HudWidget->SetAmmoText(StoredAmmo, true);
	if (bChangeWeaponImage) HudWidget->SetWeaponImage(GunHUDTexture);
}

void AGun::PlayRecoil()
{
	//Recoil Things
	SetCameraRecoil(); //Recoil CAMERA
	Playtimeline(RecoilAnimTimeline); //Recoil GUN 
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