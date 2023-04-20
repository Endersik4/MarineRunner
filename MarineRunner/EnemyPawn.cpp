// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"

#include "Bullet.h"
#include "MarineCharacter.h"


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
	

}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	CopyOfMagazineCapacity = MagazineCapacity;
	SetUpMarinePawn();
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyPawn::Shoot()
{
	if (BulletClass == NULL || bCanShoot == false) return;

	if (MagazineCapacity <= 0)
	{
		bCanShoot = false;
		//if (EmptyMagazineSound) UGameplayStatics::SpawnSoundAttached(EmptyMagazineSound, BaseSkeletalMesh, NAME_None);
		GetWorldTimerManager().SetTimer(DelayEmptyMagazineHandle, this, &AEnemyPawn::DelayAfterEmptyMagazine, DelayTimeMagazine, false);
		return;
	}

	if (ShootingSound) UGameplayStatics::SpawnSoundAttached(ShootingSound, EnemySkeletalMesh, NAME_None);
	if (ShootParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, EnemySkeletalMesh, TEXT("MuzzleFlash"), FVector(0, 0, 0), FRotator(0, 0, 0), FVector(ShootParticleScale));
	}

	//if (ShootAnimation) BaseSkeletalMesh->PlayAnimation(ShootAnimation, false);

	//Had to do this in this way because bullet was spawned before bones were on their locations (in shooting animation)
	GetWorldTimerManager().SetTimer(ImpulseOnBulletHandle, this, &AEnemyPawn::SpawnBullet, 0.001f, false);

	MagazineCapacity--;
	PlayAnimMontageInBlueprint();
	bCanShoot = false;
	
}

void AEnemyPawn::DelayAfterEmptyMagazine()
{
	MagazineCapacity = CopyOfMagazineCapacity;
	bCanShoot = true;
}

void AEnemyPawn::SpawnBullet()
{
	//FRotator BulletRotation = GetActorRotation();
	//BulletRotation.Yaw -= 1.f;
	//BulletRotation.Pitch -= 1.f;


	FRotator BulletRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(TEXT("Koncowka_Drugiego_Palca_R")), CameraLocation);
	ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, EnemySkeletalMesh->GetSocketLocation(TEXT("Bullet")), BulletRotation);
	SpawnedBullet->SetDamage(Damage);
	SpawnedBullet->SetAmmoSpeed(AmmoSpeed);
	SpawnedBullet->SetAmmoDistance(AmmoDistance);
	SpawnedBullet->SetAmmoFallingDown(AmmoFallingDown);
	SpawnedBullet->SetAmmoImpulseForce(AmmoImpulseForce);
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

