// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"

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

	Tags.Add(TEXT("Gun"));

}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	RecoilAnimTimeline = SetupTimeline(RecoilAnimTimeline, RecoilAnimCurveLocationX, FName("RecoilAnimTimeline"), FName("RecoilAnimTimelineDirection"), RecoilAnimTimelineLength, FName("RecoilAnimTimelineCallback"), FName("RecoilAnimTimelineFinishedCallback"));
	if (bShouldUseCurveRecoil) RecoilCameraTimeline = SetupTimeline(RecoilCameraTimeline, RecoilCameraCurveY, FName("RecoilCameraTimeline"), FName("RecoilCameraTimelineDirection"), RecoilCameraTimelineLength, FName("RecoilCameraTimelineCallback"), FName("RecoilCameraTimelineFinishedCallback"));
	
	CopyOfMagazineCapacity = MagazineCapacity;
	PC = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GunSway(DeltaTime);
	AimTheGun(DeltaTime);
	UpRecoilCamera(DeltaTime);
	InterpBackToInitialPosition(DeltaTime);
}

void AGun::Reload()
{
	if (StoredAmmo <= 0) return;

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

	SetWeaponInHud(true);
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

void AGun::EquipWeapon(class AMarineCharacter* Marine)
{
	BaseSkeletalMesh->SetSimulatePhysics(false);
	BaseSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarinePawn = Marine;

	//Changing Weapons things In HUD to the correct ones
	SetHudWidget(Marine->GetHudWidget());
	SetWeaponInHud(true, true);

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

	FVector DropImpulse = MarinePawn->GetCamera()->GetForwardVector() * 10 * DropImpulseDistance;
	BaseSkeletalMesh->AddImpulse(DropImpulse);
	MarinePawn = nullptr;
}

void AGun::Shoot()
{
	if (BulletClass == NULL || bCanShoot == false) return;

	if (MagazineCapacity <= 0)
	{
		if (EmptyMagazineSound) UGameplayStatics::SpawnSoundAttached(EmptyMagazineSound, BaseSkeletalMesh, NAME_None);
		ShootReleased();
		return;
	}

	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, BaseSkeletalMesh, NAME_None);
	if (ShootParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, BaseSkeletalMesh, TEXT("MuzzleFlash"), FVector(0,0,0), FRotator(0,0,0), FVector(ShootParticleScale));
	}

	if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);
	if (DropBulletClass)
	{
		AActor* DropBullet = GetWorld()->SpawnActor<AActor>(DropBulletClass, BaseSkeletalMesh->GetSocketLocation(TEXT("BulletDrop")), GetActorRotation());
		DropBullet->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, TEXT("BulletDrop"));
	}

	//Proper Location and Rotation for Bullet
	FVector Location = BaseSkeletalMesh->GetSocketLocation(TEXT("Bullet"));
	BulletRotation = GetActorRotation();
	if (PitchBulletRecoilArray.Num() == 2 && YawBulletRecoilArray.Num() == 2)
	{
		float NewPitchRotaton = FMath::FRandRange(PitchBulletRecoilArray[0], PitchBulletRecoilArray[1]);
		float NewYawRotation = FMath::FRandRange(YawBulletRecoilArray[0], YawBulletRecoilArray[1]);
		BulletRotation.Pitch += NewPitchRotaton;
		BulletRotation.Yaw += NewYawRotation;
	}

	ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, Location, BulletRotation);
	SpawnedBullet->SetDamage(Damage);
	SpawnedBullet->SetAmmoSpeed(AmmoSpeed);
	SpawnedBullet->SetAmmoDistance(AmmoDistance);
	SpawnedBullet->SetAmmoFallingDown(AmmoFallingDown);
	SpawnedBullet->SetAmmoImpulseForce(AmmoImpulseForce);
	SpawnedBullet->ImpulseOnBullet();

	MagazineCapacity--;
	SetWeaponInHud();

	bCanShoot = false;

	if (bCanRecoilCamera == false)
	{
		bCanRecoilCamera = true;
		InitialCameraRotation = PC->GetControlRotation();
		if (bShouldUseCurveRecoil) Playtimeline(RecoilCameraTimeline);
		else if ((PitchRecoilRangeArray.Num() == 2 && YawRecoilRangeArray.Num() == 2))
		{
			RandomRecoilYaw = FMath::FRandRange(YawRecoilRangeArray[0], YawRecoilRangeArray[1]);

			RandomRecoilPitch = FMath::FRandRange(PitchRecoilRangeArray[0], PitchRecoilRangeArray[1]);
		}
	}

	bCanSway = false;
	bCanDropTheGun = false;
	Playtimeline(RecoilAnimTimeline);
}

void AGun::ShootReleased()
{
	if (bCanRecoilCamera == true)
	{
		GetWorldTimerManager().ClearTimer(FixingBugHandle);
		bCanRecoilCamera = false;
		TimeRecoilCameraElapsed = 0.f;
		bConstantlyShoot = false;
		if (bShouldUseCurveRecoil) RecoilCameraTimeline->Stop();

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
			//Oblicza dystans miedzy dwoma FRotator #include "Kismet/KismetMathLibrary.h"
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
		else if (PitchRecoilRangeArray.Num() == 2)
		{
			if (DistanceBetweenPitch > PitchRecoilRangeArray[1])
			{
				InitialCameraRotation = PC->GetControlRotation();
				InitialCameraRotation.Pitch -= PitchRecoilRangeArray[1];
			}
		}
		
		bShouldInterpBack = true;
	}
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

void AGun::RecoilAnimTimelineCallback(float RecoilDirection)
{
	if (RecoilAnimCurvePitch)
	{
		float RecoilPitch = RecoilAnimCurvePitch->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		if (CanAimTheGun == 1) RecoilPitch /= DividerOfRecoilWhileAiming;

		FRotator GunMeshRotation = BaseSkeletalMesh->GetRelativeRotation();
		GunMeshRotation.Pitch = RecoilPitch * MultiplierOfRecoilCurvePitch;

		BaseSkeletalMesh->SetRelativeRotation(GunMeshRotation);
	}

	if (RecoilAnimCurveScale)
	{
		float RecoilScale = RecoilAnimCurveScale->GetFloatValue(RecoilAnimTimeline->GetPlaybackPosition());
		BaseSkeletalMesh->SetRelativeScale3D(FVector(RecoilScale));
	}

	if (CanAimTheGun == 1)
	{
		RecoilDirection /= DividerOfRecoilWhileAiming;
	}
	
	FVector GunMeshLocation = BaseSkeletalMesh->GetRelativeLocation();
	GunMeshLocation.X = GunMeshLocation.X - RecoilDirection;

	BaseSkeletalMesh->SetRelativeLocation(GunMeshLocation);
}

void AGun::RecoilAnimTimelineFinishedCallback()
{
	bCanShoot = true;
	bCanSway = true;
	if (CanAimTheGun != 1) bCanDropTheGun = true;
	if (bConstantlyShoot)
	{
		//I need to use a timer to prevent an error that causes the bullet to appear in a different location than the Bullet Socket.
		GetWorldTimerManager().SetTimer(FixingBugHandle, this, &AGun::Shoot, 0.001f);
	}
}

void AGun::RecoilCameraTimelineCallback(float ControlRotationY)
{
	PC->AddYawInput(ControlRotationY);
}

UTimelineComponent* AGun::SetupTimeline(UTimelineComponent* TimeLineComp, UCurveFloat* MostImportantCurve, FName TimeLineName, FName TimeLineDirection, float TimeLineLength,
	FName TimelineCallbackFunction, FName TimelineFinishedFunction)
{
	if (!MostImportantCurve) return nullptr;

	FOnTimelineFloat onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	TimeLineComp = NewObject<UTimelineComponent>(this, TimeLineName); //Tworzenie obiektu, cos jak CreateDefaultSubobject
	TimeLineComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	this->BlueprintCreatedComponents.Add(TimeLineComp); //Dodanie obiektu do Array aby ten sie zapisal

	TimeLineComp->SetPropertySetObject(this); //ustawienie obiektu na ktorym timeline ma sterowac wlasciwosciami
	TimeLineComp->SetDirectionPropertyName(TimeLineDirection); //Ustawienie zmiennej RecoilTimelineDirection do Timeline

	TimeLineComp->SetLooping(false); //Czy petla
	TimeLineComp->SetTimelineLength(TimeLineLength); //UStawienie dlugosci Timeline'u
	TimeLineComp->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength); //Kiedy ma sie petla skonczyc czy po danym czasie 
	//czy Po ostatnim keyframe ETimelineLengthMode::TL_LastKeyFrame

	onTimelineCallback.BindUFunction(this, TimelineCallbackFunction); //Bindowanie funkcji ktora ma sie odapalc za kazdym 
	//razem gdy wartosc w Curve sei zmieni
	onTimelineFinishedCallback.BindUFunction(this, TimelineFinishedFunction); //To samo co wyzej tylko ze jak sie 
	//Timeline skonczy
	TimeLineComp->AddInterpFloat(MostImportantCurve, onTimelineCallback); //Wartosc tej krzywej bedzie zapisana w tej funkcji Callback
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

void AGun::UpRecoilCamera(float Delta)
{
	if (bCanRecoilCamera == false) return;

	if (bShouldUseCurveRecoil)
	{
		float ControlRotationPitch = (DistanceFromStart * 0.375) * TimeRecoilCameraElapsed / ((CopyOfMagazineCapacity * RecoilAnimTimelineLength) + 0.2f);
		PC->AddPitchInput(-ControlRotationPitch);
	}
	else
	{
		float ControlRotationPitch = (RandomRecoilPitch * 0.375) * TimeRecoilCameraElapsed / RecoilAnimTimelineLength;
		float ControlRotationYaw = (RandomRecoilYaw * 0.375) * TimeRecoilCameraElapsed / RecoilAnimTimelineLength;
		
		PC->AddYawInput(ControlRotationYaw);
		PC->AddPitchInput(-ControlRotationPitch);
	}
	TimeRecoilCameraElapsed = Delta;

}

void AGun::InterpBackToInitialPosition(float Delta)
{
	if (!MarinePawn) return;
	
	if (bShouldInterpBack == false) return;

	if (PC->MouseXValue > 0.05f || PC->MouseXValue < -0.05f || PC->MouseYValue > 0.05f || PC->MouseYValue < -0.05f)
	{
		bShouldInterpBack = false;
		return;
	}

	if (PC->GetControlRotation().Equals(InitialCameraRotation, 0.2f))
	{
		bShouldInterpBack = false;
		return;
	}

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(PC->GetControlRotation(), InitialCameraRotation, Delta, InitalCameraPositionSpeed);

	PC->SetControlRotation(NewRotation);
}

void AGun::GunSway(float Delta)
{
	if (MarinePawn == nullptr || bCanGunSwayTick == false || bCanSway == false || CanAimTheGun == 1) return;

	float LookUp = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputAxisValue("LookUp");
	float LookRight = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputAxisValue("LookRight");
	float Forward = MarinePawn->GetInputAxisValue("Forward");
	float Right = MarinePawn->GetInputAxisValue("Right");
	
	float InterpLookUp = FMath::FInterpTo(GunRotationSway.Pitch, UKismetMathLibrary::MapRangeClamped(LookUp, -1, 1, RotationSwayPitchRangeBack, RotationSwayPitchRangeUp), Delta, SpeedOfSwayPitch);
	float InterpLookRight = FMath::FInterpTo(GunRotationSway.Yaw, UKismetMathLibrary::MapRangeClamped(LookRight, -1, 1, RotationSwayYawRangeBack, RotationSwayYawRangeUp), Delta, SpeedOfSwayYaw);

	FVector GunLocationInterp = BaseSkeletalMesh->GetRelativeLocation(); 
	float InterpForward = FMath::InterpEaseInOut(GunLocationInterp.X, UKismetMathLibrary::MapRangeClamped(Forward, 1, -1, RelativeLocationInPawn.X + LocationSwayXRangeBack, 
		RelativeLocationInPawn.X + LocationSwayXRangeUp), (Delta * SpeedOfSwayX), 1.f);
	float InterpRight = FMath::InterpEaseInOut(GunLocationInterp.Y, UKismetMathLibrary::MapRangeClamped(Right, -1, 1, RelativeLocationInPawn.Y + LocationSwayYRangeBack,
		RelativeLocationInPawn.Y + LocationSwayYRangeUp), (Delta * SpeedOfSwayY), 1.f);

	GunRotationSway = FRotator(InterpLookUp, InterpLookRight, 0.f);
	GunLocationInterp = FVector(InterpForward, InterpRight, GunLocationInterp.Z);

	BaseSkeletalMesh->SetRelativeLocation(GunLocationInterp);
	BaseSkeletalMesh->SetRelativeRotation(GunRotationSway);

	if (MarinePawn->GetInputAxisValue("Forward") == 0 && MarinePawn->GetInputAxisValue("Right") == 0)
	{
		FVector cos = FMath::VInterpTo(BaseSkeletalMesh->GetRelativeLocation(), RelativeLocationInPawn, Delta, 1.f);
		BaseSkeletalMesh->SetRelativeLocation(cos);
	}
}

void AGun::GunSwayWhileMoving()
{
	//Using Lemniscate Of Bernoulli to sway gun while moving 
	if (bCanSway == false || MarinePawn == nullptr) return;
	if (MarinePawn->GetInputAxisValue("Forward") == 0 && MarinePawn->GetInputAxisValue("Right") == 0) return;

	float SpeedOfLemniscate = GetWorld()->GetTimeSeconds() * SpeedOfSwayWhileMoving;

	float Angle = 2 / (9.f - FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate * 2)));
	float LocationY = Angle * FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate)); //SineWave
	float LocationZ = (FMath::Sin(FMath::DegreesToRadians(SpeedOfLemniscate * 2)) * Angle) / 2; //Angle Offset

	if (CanAimTheGun == 1)
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
	if (CanAimTheGun == 0) return;

	FVector Location = BaseSkeletalMesh->GetRelativeLocation();
	FRotator Rotation = BaseSkeletalMesh->GetRelativeRotation();
	if (CanAimTheGun == 1)
	{
		BaseSkeletalMesh->SetRelativeLocation(FMath::Lerp(Location, RelativeLocationInPawnWhileAiming, Delta * SpeedOfAiming));
		BaseSkeletalMesh->SetRelativeRotation(FMath::RInterpTo(Rotation, RelativeRotationInPawn, Delta, SpeedOfAiming));
		bCanDropTheGun = false;
	}
	else
	{
		BaseSkeletalMesh->SetRelativeLocation(FMath::Lerp(Location, RelativeLocationInPawn, Delta * SpeedOfAimingBack));
		bool bIsLerpEnded = BaseSkeletalMesh->GetRelativeLocation().Equals(RelativeLocationInPawn, 1.5f);
		if (bIsLerpEnded)
		{
			bCanDropTheGun = true;
			CanAimTheGun = 0;
		}
	}
}