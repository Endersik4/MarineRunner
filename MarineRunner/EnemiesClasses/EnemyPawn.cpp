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
	
	Tags.Add(TEXT("enemy"));
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

void AEnemyPawn::Shoot()
{
	if (BulletClass == NULL || bCanShoot == false || bIsDead == true) return;
	
	if (MagazineCapacity <= 0)
	{
		Reload();
		return;
	}

	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, EnemySkeletalMesh, TEXT("MuzzleFlash"));
	if (ShootParticle)
	{
		UParticleSystemComponent* SpawnedShootParticle = UGameplayStatics::SpawnEmitterAttached(ShootParticle, EnemySkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));	
	}

	//Had to do this in this way because bullet was spawned before bones were on their locations (in shooting animation)
	GetWorldTimerManager().SetTimer(ImpulseOnBulletHandle, this, &AEnemyPawn::SpawnManyBullets, 0.001f, false);

	MagazineCapacity--;
	PlayAnimMontageInBlueprint();
	bCanShoot = false;

	FVector RecoilImpulse = -GetActorForwardVector() * RecoilImpulseOnEnemy * 100.f;
	CapsuleColl->AddImpulse(RecoilImpulse);
}


//void AEnemyPawn::BreakObject_Implementation(float NewDamage)
//{
//	UE_LOG(LogTemp, Warning, TEXT("IMPLEMENTATION"));
//}

void AEnemyPawn::ApplyDamage(float NewDamage, float NewImpulse, FVector ImpulseDirection, const FHitResult& NewHit)
{
	Health -= NewDamage;
	SpawnEffectsForImpact(NewHit);
	SpawnBloodDecal(NewHit);

	//Kill enemy
	if (Health <= 0.f)
	{
		EnemySkeletalMesh->SetSimulatePhysics(true);
		bIsDead = true;

		FVector Impulse = ImpulseDirection * (AmmoImpulseForce * 100);
		EnemySkeletalMesh->AddImpulse(Impulse, NewHit.BoneName);

		EnemySkeletalMesh->Stop();
		return;
	}

	if (Health <= 10.f)
	{
		float ShouldRunAwayRandom = FMath::FRandRange(0.f, 100.f);
		if (ShouldRunAwayRandom <= 30.f)
		{
			ShouldRunAway();
			return;
		}
	}

	AlertEnemyAboutPlayer();
}

void AEnemyPawn::SpawnEffectsForImpact(const FHitResult& Hit)
{
	if (EnemyHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), EnemyHitSound, Hit.ImpactPoint);
	if (!EnemyBloodParticle) return;

	FRotator Rotation = Hit.ImpactNormal.Rotation() - FRotator(90.f, 0.f, 0.f);
	UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyBloodParticle, Hit.ImpactPoint, Rotation);
	SpawnedParticle->SetColorParameter(TEXT("ColorOfBlood"), BloodColor);
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

void AEnemyPawn::CheckIfEnemySeePlayer()
{
	if (!EnemyAIController || bIsDead == true || !MarinePawn || bIsRunningAway) return;
	if (EnemyAIController->GetDoEnemySeePlayer() == false) return;

	PredictWhereToShoot();
	FocusBonesOnPlayerWhenPlayerDetected();
}

FRotator AEnemyPawn::FocusBoneOnPlayer(FName BoneName, bool bLookStraight)
{
	FRotator BoneRotation;
	FRotator FoundRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneName), 
		(bLookStraight) ? MarinePawn->GetCamera()->GetComponentLocation() : CameraLocation);
	BoneRotation.Roll = FoundRotation.Pitch * -1.f;
	BoneRotation.Yaw = FoundRotation.Yaw - GetActorRotation().Yaw;

	return BoneRotation;
}

void AEnemyPawn::PredictWhereToShoot()
{
	CameraLocation = MarinePawn->GetCamera()->GetComponentLocation();
	if (MarinePawn->GetInputAxisValue("Right") == -1.f)
	{
		CameraLocation += MarinePawn->GetCamera()->GetRightVector() * 150.f;
	}
	else if (MarinePawn->GetInputAxisValue("Right") == 1.f)
	{
		CameraLocation -= MarinePawn->GetCamera()->GetRightVector() * 150.f;
	}
}

void AEnemyPawn::SetIsDead(bool bNewDead)
{
	bIsDead = bNewDead;
	EnemyAIController->KillEnemy();
}

void AEnemyPawn::ShouldRunAway()
{
	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
	EnemyAIController->RunAway();
}

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

void AEnemyPawn::SpawnManyBullets()
{
	if (bManyBulletAtOnce) for (int i = 0; i != HowManyBulletsToSpawn; i++) SpawnBullet();
	else SpawnBullet();
}

void AEnemyPawn::SpawnBullet()
{
	FRotator BulletRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneNameForBulletRotation), CameraLocation);
	//Bullet will randomly "go" to other directions 
	if (PitchBulletRecoilArray.Num() == 2 && YawBulletRecoilArray.Num() == 2 && bManyBulletAtOnce)
	{
		float NewPitchRotaton = FMath::FRandRange(PitchBulletRecoilArray[0], PitchBulletRecoilArray[1]);
		float NewYawRotation = FMath::FRandRange(YawBulletRecoilArray[0], YawBulletRecoilArray[1]);
		BulletRotation.Pitch += NewPitchRotaton;
		BulletRotation.Yaw += NewYawRotation;
	}
	ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, EnemySkeletalMesh->GetSocketLocation(TEXT("Bullet")), BulletRotation);
	
	float BulletDamage = (bManyBulletAtOnce == false ? Damage : Damage / HowManyBulletsToSpawn);
	SpawnedBullet->SetBulletVariables(BulletDamage, AmmoSpeed, AmmoDistance, AmmoFallingDown, AmmoImpulseForce);
	SpawnedBullet->ImpulseOnBullet();
}

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

