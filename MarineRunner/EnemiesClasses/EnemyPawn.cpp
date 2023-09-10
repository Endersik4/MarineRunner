// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/EnemiesClasses/EnemyAiController.h"


// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleColl = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	RootComponent = CapsuleColl;
	bUseControllerRotationYaw = true;
	//CapsuleColl->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//CapsuleColl->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//CapsuleColl->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	EnemyFloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("EnemyFloatingMovement"));
	
	EnemySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemySkeletalMesh"));
	EnemySkeletalMesh->SetupAttachment(CapsuleColl);
	EnemySkeletalMesh->SetSimulatePhysics(false);
	EnemySkeletalMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	EnemySkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	CopyOfMagazineCapacity = MagazineCapacity;
	SetUpMarinePawn();
	SetUpEnemyAIController();
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIfEnemySeePlayer();
	PlayFootstepsSound();
}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

#pragma region ///////////////// SHOOT //////////////////
//Shoot function is executed in ShootAndMove.cpp
void AEnemyPawn::Shoot()
{
	if (BulletData.BulletClass == NULL || bCanShoot == false || bIsDead == true) return;
	
	if (MagazineCapacity <= 0)
	{
		Reload();
		return;
	}

	ShootEffects();

	//Had to do this in this way because bullet was spawned before bones were on their locations (in shooting animation)
	GetWorldTimerManager().SetTimer(ImpulseOnBulletHandle, this, &AEnemyPawn::SpawnManyBullets, 0.001f, false);

	MagazineCapacity--;
	PlayAnimMontageInBlueprint();
	bCanShoot = false;

	PushBackDuringShooting();
}

void AEnemyPawn::ShootEffects()
{
	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, EnemySkeletalMesh, TEXT("MuzzleFlash"));
	if (ShootParticle)
	{
		UParticleSystemComponent* SpawnedShootParticle = UGameplayStatics::SpawnEmitterAttached(ShootParticle, EnemySkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	}
}

void AEnemyPawn::PushBackDuringShooting()
{
	FVector RecoilImpulse = -GetActorForwardVector() * RecoilImpulseOnEnemy * 100.f;
	CapsuleColl->AddImpulse(RecoilImpulse);
}

void AEnemyPawn::PredictWhereToShoot()
{
	PlayerCameraLocation = MarinePawn->GetCamera()->GetComponentLocation();

	if (MarinePawn->GetInputAxisValue("Right") == -1.f)
	{
		PlayerCameraLocation += MarinePawn->GetCamera()->GetRightVector() * 150.f;
	}
	else if (MarinePawn->GetInputAxisValue("Right") == 1.f)
	{
		PlayerCameraLocation -= MarinePawn->GetCamera()->GetRightVector() * 150.f;
	}

	if (bShouldAlsoPredictVertical == false) return;

	PlayerCameraLocation.Z += FVector::Distance(GetActorLocation(), MarinePawn->GetActorLocation()) / 10;
}
#pragma endregion

#pragma region ///////////////// DAMAGE ///////////////////////
void AEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	Health -= NewDamage;
	SpawnEffectsForImpact(NewHit);
	SpawnBloodDecal(NewHit);

	bool bEnemyKilled = KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);
	if (bEnemyKilled == true) return;

	bool bEnemyIsRunningAway = EnemyRunAway();
	if (bEnemyIsRunningAway == true) return;

	AlertEnemyAboutPlayer();
}

bool AEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (Health > 0.f) return false;
	EnemySkeletalMesh->SetSimulatePhysics(true);
	bIsDead = true;

	if (NewSphereRadius != 0.f)
	{
		EnemySkeletalMesh->AddRadialImpulse(BulletActor->GetActorLocation(), NewSphereRadius, NewImpulseForce * 10.f, ERadialImpulseFalloff::RIF_Linear);
	}
	else
	{
		EnemySkeletalMesh->AddImpulse(BulletActor->GetActorForwardVector() * NewImpulseForce * 10.f, NewHit.BoneName);
	}

	EnemySkeletalMesh->Stop();
	return true;
}

bool AEnemyPawn::EnemyRunAway()
{
	if (Health > 10.f || bCanEnemyRunAway == false) return false;

	float ShouldRunAwayRandom = FMath::FRandRange(0.f, 100.f);
	if (ShouldRunAwayRandom <= 30.f)
	{
		ShouldRunAway();
		return true;
	}

	return false;
}

void AEnemyPawn::AlertEnemyAboutPlayer()
{
	if (!EnemyAIController) return;

	if (EnemyAIController->GetDoEnemySeePlayer() == true) return;

	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	float FoundRotationYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation).Yaw;

	FRotator EnemyRotation = GetActorRotation();
	EnemyRotation.Yaw = FoundRotationYaw;
	SetActorRotation(EnemyRotation);
}
#pragma endregion 

#pragma region ///////////// BULLETS /////////////////////

void AEnemyPawn::SpawnBullet()
{
	FRotator BulletRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneNameForBulletRotation), PlayerCameraLocation);
	//Bullet will randomly "go" to other directions 
	if (PitchBulletRecoilArray.Num() == 2 && YawBulletRecoilArray.Num() == 2 && bManyBulletAtOnce)
	{
		float NewPitchRotaton = FMath::FRandRange(PitchBulletRecoilArray[0], PitchBulletRecoilArray[1]);
		float NewYawRotation = FMath::FRandRange(YawBulletRecoilArray[0], YawBulletRecoilArray[1]);
		BulletRotation.Pitch += NewPitchRotaton;
		BulletRotation.Yaw += NewYawRotation;
	}

	FTransform BulletTransform = FTransform(BulletRotation, EnemySkeletalMesh->GetSocketLocation(TEXT("Bullet")));
	ABullet* SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletData.BulletClass, BulletTransform);

	BulletData.Damage = (bManyBulletAtOnce == false ? BulletData.Damage : BulletData.Damage / HowManyBulletsToSpawn);

	SpawnedBullet->SetBulletData(BulletData);
	SpawnedBullet->FinishSpawning(BulletTransform);
}

void AEnemyPawn::SpawnManyBullets()
{
	if (bManyBulletAtOnce) for (int i = 0; i != HowManyBulletsToSpawn; i++) SpawnBullet();
	else SpawnBullet();
}
#pragma endregion

#pragma region ///////////// EFFECTS ////////////////////
void AEnemyPawn::SpawnBloodDecal(const FHitResult& Hit)
{
	if (!BloodDecalMaterial) return;

	FVector Size = FVector(FMath::FRandRange(5.f, 20.f));
	FRotator Rotation = Hit.ImpactNormal.Rotation();
	UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAttached(BloodDecalMaterial, Size, EnemySkeletalMesh, Hit.BoneName, Hit.Location, Rotation, EAttachLocation::KeepWorldPosition);
	if (SpawnedDecal) SpawnedDecal->SetFadeScreenSize(0.f);
}

void AEnemyPawn::PlayFootstepsSound()
{
	if (!FootstepsSound) return;
	if (GetVelocity().Length() < 1.f || bCanPlayFootstepsSound == false || bIsDead) return;

	float FootstepsTime = 0.42f;
	if (bIsRunningAway)
	{
		if (FootstepsRunningAwaySound) UGameplayStatics::SpawnSoundAttached(FootstepsRunningAwaySound, EnemySkeletalMesh);
		FootstepsTime = 0.2f;
	}
	else UGameplayStatics::SpawnSoundAttached(FootstepsSound, EnemySkeletalMesh);

	bCanPlayFootstepsSound = false;
	GetWorldTimerManager().SetTimer(FootstepsHandle, this, &AEnemyPawn::SetCanPlayFootstepsSound, FootstepsTime, false);
}

void AEnemyPawn::SpawnEffectsForImpact(const FHitResult& Hit)
{
	if (EnemyHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), EnemyHitSound, Hit.ImpactPoint);
	if (!EnemyBloodParticle) return;

	FRotator Rotation = Hit.ImpactNormal.Rotation() - FRotator(90.f, 0.f, 0.f);
	UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyBloodParticle, Hit.ImpactPoint, Rotation);
	SpawnedParticle->SetColorParameter(TEXT("ColorOfBlood"), BloodColor);
}
#pragma endregion

#pragma region ////////////// ENEMY SEE PLAYER //////////////
bool AEnemyPawn::ConditionsForEnemyToSeePlayer()
{
	if (!EnemyAIController || bIsDead == true || !MarinePawn || bIsRunningAway) return false;
	if (EnemyAIController->GetDoEnemySeePlayer() == false) return false;

	return true;
}

void AEnemyPawn::CheckIfEnemySeePlayer()
{
	if (ConditionsForEnemyToSeePlayer() == false) return;

	PredictWhereToShoot();
	FocusBonesOnPlayerWhenPlayerDetected();
}
#pragma endregion

FRotator AEnemyPawn::FocusBoneOnPlayer(FName BoneName, bool bLookStraight)
{
	FRotator BoneRotation;
	FRotator FoundRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneName), 
		(bLookStraight) ? MarinePawn->GetCamera()->GetComponentLocation() : PlayerCameraLocation);
	BoneRotation.Roll = FoundRotation.Pitch * -1.f;
	BoneRotation.Yaw = FoundRotation.Yaw - GetActorRotation().Yaw;

	return BoneRotation;
}

void AEnemyPawn::ShouldRunAway()
{
	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
	EnemyAIController->RunAway();
}

#pragma region ///////////////// RELOAD /////////////////
void AEnemyPawn::Reload()
{
	bIsReloading = true;
	bCanShoot = false;
	GetWorldTimerManager().SetTimer(DelayEmptyMagazineHandle, this, &AEnemyPawn::DelayAfterEmptyMagazine, DelayTimeMagazine, false);
}

void AEnemyPawn::DelayAfterEmptyMagazine()
{
	MagazineCapacity = CopyOfMagazineCapacity;
	bCanShoot = true;
	bIsReloading = false;
}
#pragma endregion

#pragma region /////////// SETTERS ////////////////
void AEnemyPawn::SetIsDead(bool bNewDead)
{
	bIsDead = bNewDead;
	EnemyAIController->KillEnemy();
}

void AEnemyPawn::SetUpMarinePawn()
{
	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!MarinePawn)
	{
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN CAST ERROR IN ENEMY PAWN"));
	}
}

void AEnemyPawn::SetUpEnemyAIController()
{
	EnemyAIController = Cast<AEnemyAiController>(GetController());
	if (!EnemyAIController)
	{
		UE_LOG(LogTemp, Error, TEXT("ENEMY AI CONTROLER CAST ERROR IN ENEMY PAWN"));
	}
}
#pragma endregion
