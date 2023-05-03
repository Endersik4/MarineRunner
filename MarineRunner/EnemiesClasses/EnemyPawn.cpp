// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"

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

	EnemyFloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("EnemyFloatingMovement"));
	
	EnemySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemySkeletalMesh"));
	EnemySkeletalMesh->SetupAttachment(CapsuleColl);
	EnemySkeletalMesh->SetSimulatePhysics(false);
	EnemySkeletalMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	
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

	if (ShootingSound) 	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootingSound, EnemySkeletalMesh->GetSocketLocation("MuzzleFlash"), FRotator::ZeroRotator, 1.f, 1.f, 0.f, ShootingAttenuation);
	if (ShootParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, EnemySkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	}

	//Had to do this in this way because bullet was spawned before bones were on their locations (in shooting animation)
	GetWorldTimerManager().SetTimer(ImpulseOnBulletHandle, this, &AEnemyPawn::SpawnBullet, 0.001f, false);

	MagazineCapacity--;
	PlayAnimMontageInBlueprint();
	bCanShoot = false;
	
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

void AEnemyPawn::SetShouldRunningAway()
{
	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
	EnemyAIController->RunAway();
}

void AEnemyPawn::Reload()
{
	bIsReloading = true;
	bCanShoot = false;
	//if (EmptyMagazineSound) UGameplayStatics::SpawnSoundAttached(EmptyMagazineSound, BaseSkeletalMesh, NAME_None);
	GetWorldTimerManager().SetTimer(DelayEmptyMagazineHandle, this, &AEnemyPawn::DelayAfterEmptyMagazine, DelayTimeMagazine, false);
}

void AEnemyPawn::DelayAfterEmptyMagazine()
{
	MagazineCapacity = CopyOfMagazineCapacity;
	bCanShoot = true;
	bIsReloading = false;
}

void AEnemyPawn::SpawnBullet()
{
	FRotator BulletRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(TEXT("Koncowka_Drugiego_Palca_R")), CameraLocation);
	ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, EnemySkeletalMesh->GetSocketLocation(TEXT("Bullet")), BulletRotation);
	
	SpawnedBullet->SetBulletVariables(Damage, AmmoSpeed, AmmoDistance, AmmoFallingDown, AmmoImpulseForce);
	SpawnedBullet->ImpulseOnBullet();
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

