// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/EnemiesClasses/EnemyAiController.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;

	//BulletMesh->SetMassScale(NAME_None, 1.5f);
	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); //For the enemy bullet set this to BLOCK

	OnActorHit.AddDynamic(this, &ABullet::OnHit);
	Tags.Add(TEXT("Bullet"));
}	

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementBullet(DeltaTime);
}

void ABullet::MovementBullet(float Delta)
{
	if (bUseMyMovement == false) return;

	BulletLocation = GetActorLocation() + (GetActorForwardVector() * AmmoSpeed * Delta);
	
	if (TrackBulletDistance >= AmmoDistance)
	{
		BulletLocation -= GetActorUpVector() * (AmmoFallingDown * Delta);
		AmmoFallingDown += 20 * Delta;
	}
	else
	{
		TrackBulletDistance += (AmmoSpeed * Delta);
	}

	SetActorLocation(BulletLocation, true);
}

void ABullet::ImpulseOnBullet()
{
	if (bUseImpulseForBullet)
	{
		BulletMesh->SetSimulatePhysics(true);
		FVector Impulse = GetActorForwardVector() * AmmoSpeed;
		BulletMesh->AddImpulse(Impulse);
	}
	else bUseMyMovement = true;
}

void ABullet::DamageEnemy(AEnemyPawn* Enemy, const FHitResult& Hit)
{
	Enemy->SetHealth(Enemy->GetHealth() - Damage);

	//Kill enemy
	if (Enemy->GetHealth() <= 0.f)
	{
		Enemy->GetEnemySkeletalMesh()->SetSimulatePhysics(true);
		Enemy->SetIsDead(true);

		FVector Impulse = GetActorForwardVector() * (AmmoImpulseForce * 100);
		Enemy->GetEnemySkeletalMesh()->AddImpulse(Impulse, Hit.BoneName);

		Enemy->GetEnemySkeletalMesh()->Stop();
		return;
	}

	if (Enemy->GetHealth() <= 10.f)
	{
		Enemy->SetShouldRunningAway();
		return;
	}

	AlertEnemyAboutPlayer(Enemy);
}

void ABullet::AlertEnemyAboutPlayer(AEnemyPawn* Enemy)
{
	AEnemyAiController* EnemyAIController = Cast<AEnemyAiController>(Enemy->GetController());
	if (!EnemyAIController) return;

	if (EnemyAIController->GetDoEnemySeePlayer() == false)
	{
		FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
		float FoundRotationYaw = UKismetMathLibrary::FindLookAtRotation(Enemy->GetActorLocation(), PlayerLocation).Yaw;

		FRotator EnemyRotation = Enemy->GetActorRotation();
		EnemyRotation.Yaw = FoundRotationYaw;
		Enemy->SetActorRotation(EnemyRotation);
	}
}

void ABullet::SpawnBulletHole(const FHitResult& Hit)
{
	if (!BulletHoleDecalMaterial) return;

	FVector Size = FVector(FMath::FRandRange(7.f, 8.f));
	FRotator Rotation = Hit.ImpactNormal.Rotation();
	UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecalMaterial, Size, Hit.Location, Rotation);
	if (SpawnedDecal)
	{
		SpawnedDecal->SetFadeScreenSize(0.f);
		SpawnedDecal->SetLifeSpan(10.f);
	}
}

void ABullet::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Enemy"))
	{
		AEnemyPawn* Enemy = Cast<AEnemyPawn>(OtherActor);
		if (!Enemy) return;

		SpawnEffectsForImpact(Hit, TypeOfObject::EnemyType, Enemy->GetBloodColor());
		
		Enemy->SpawnBloodDecal(Hit);
		DamageEnemy(Enemy, Hit);
	}
	else if (OtherActor->ActorHasTag("Player"))
	{
		AMarineCharacter* MarinePawn = Cast<AMarineCharacter>(OtherActor);
		if (!MarinePawn) return;

		if (MarineHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MarineHitSound, Hit.ImpactPoint);

		MarinePawn->GotDamage(Damage);
	}
	else
	{
		if (Hit.GetActor()->ActorHasTag("Breakable"))
		{
			UGeometryCollectionComponent* GeometryCollection = Cast<UGeometryCollectionComponent>(Hit.GetComponent());
			GeometryCollection->ApplyExternalStrain(Hit.Item, Hit.ImpactPoint, 20.f, 1, 1.f, 5000.f);
			GeometryCollection->ApplyBreakingLinearVelocity(Hit.Item, Hit.ImpactPoint);
			GeometryCollection->AddRadialImpulse(GetActorLocation(), 100.f, AmmoImpulseForce * 100, ERadialImpulseFalloff::RIF_Linear);

			if (Hit.GetActor()->ActorHasTag("Glass")) SpawnEffectsForImpact(Hit, TypeOfObject::GlassType);
			else SpawnEffectsForImpact(Hit);
		}
		else
		{
			SpawnEffectsForImpact(Hit);
			SpawnBulletHole(Hit);
		}
	}

	SetActorTickEnabled(false);
	Destroy();
}

void ABullet::SpawnEffectsForImpact(const FHitResult& Hit, TypeOfObject Type, FLinearColor EnemyBloodColor)
{
	if (Type == TypeOfObject::GlassType)
	{
		if (GlassHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), GlassHitSound, Hit.ImpactPoint);
		if (GlassBreakParticle)
		{
			FRotator Rotation = GetActorRotation();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GlassBreakParticle, Hit.ImpactPoint, Rotation, FVector(4.f));
		}
		return;
	}

	if (Type == TypeOfObject::EnemyType)
	{
		if (EnemyHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), EnemyHitSound, Hit.ImpactPoint);
		if (EnemyBloodParticle)
		{
			FRotator Rotation = Hit.ImpactNormal.Rotation() - FRotator(90.f, 0.f, 0.f);
			UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyBloodParticle, Hit.ImpactPoint, Rotation);
			SpawnedParticle->SetColorParameter(TEXT("ColorOfBlood"), EnemyBloodColor);
		}
		return;
	}

	if (ObjectHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ObjectHitSound, Hit.ImpactPoint);
	if (BulletHitParticle)
	{
		FRotator Rotation = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticle, Hit.ImpactPoint, Rotation);
		if (BulletHit2Particle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHit2Particle, Hit.ImpactPoint, Rotation);
		}
	}
}

void ABullet::SetBulletVariables(float NewDamage, float NewAmmoSpeed, float NewAmmoDistance, float NewAmmoFallingDown, float NewAmmoImpulseForce)
{
	Damage = NewDamage;
	AmmoSpeed = NewAmmoSpeed;
	AmmoDistance = NewAmmoDistance;
	AmmoFallingDown = NewAmmoFallingDown;
	AmmoImpulseForce = NewAmmoImpulseForce;
}


