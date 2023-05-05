// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteActor.h"

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

	BulletTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletTrail"));
	BulletTrail->SetupAttachment(BulletMesh);

	OnActorHit.AddDynamic(this, &ABullet::OnHit);
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

		//There is some bug that SkeletonMesh still playing animation after Stop() but when i call 2 times this function, Skeleton stop playing animation
		Enemy->GetEnemySkeletalMesh()->Stop();
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
	if (!BulletHoleSprite) return;

	FVector Location = Hit.ImpactPoint + (-GetActorForwardVector() * 3.f);
	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += 90.f;

	APaperSpriteActor* BulletHole = GetWorld()->SpawnActor<APaperSpriteActor>(APaperSpriteActor::StaticClass(), Location, Rotation);

	if (!BulletHole) return;

	BulletHole->GetRenderComponent()->SetMobility(EComponentMobility::Movable);
	BulletHole->GetRenderComponent()->SetSprite(BulletHoleSprite);
	BulletHole->SetActorScale3D(FVector(0.01));
}

void ABullet::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BulletHits)
	{
		HitParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletHits, Hit.ImpactPoint);
	}

	if (OtherActor->ActorHasTag("Enemy"))
	{
		AEnemyPawn* Enemy = Cast<AEnemyPawn>(OtherActor);
		if (!Enemy) return;

		FColor BloodColor = Enemy->GetBloodColor();
		//I cant just do SetColorParameter(TEXT("BloodColor"), Enemy->GetBloodColor()); because of some bug it doesnt work, even when 
		//Enemy->GetBloodColor() returns FLinearColor. It just dont work
		HitParticle->SetColorParameter(TEXT("BloodColor"), FLinearColor(BloodColor.R, BloodColor.G, BloodColor.B));
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), EnemyHitSound, Hit.ImpactPoint);

		DamageEnemy(Enemy, Hit);
	}
	else if (OtherActor->ActorHasTag("Player"))
	{
		HitParticle->SetColorParameter(TEXT("BloodColor"), FLinearColor(0.f, 98.4, 100.f)); //color from HUD (light blue)

		AMarineCharacter* MarinePawn = Cast<AMarineCharacter>(OtherActor);
		if (!MarinePawn) return;

		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MarineHitSound, Hit.ImpactPoint);
		MarinePawn->GotDamage(Damage);
	}
	else if (HitParticle)
	{
		HitParticle->SetColorParameter(TEXT("BloodColor"), FLinearColor::Yellow);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ObjectHitSound, Hit.ImpactPoint);

		SpawnBulletHole(Hit);
	}

	Destroy();
}

void ABullet::SetBulletVariables(float NewDamage, float NewAmmoSpeed, float NewAmmoDistance, float NewAmmoFallingDown, float NewAmmoImpulseForce)
{
	Damage = NewDamage;
	AmmoSpeed = NewAmmoSpeed;
	AmmoDistance = NewAmmoDistance;
	AmmoFallingDown = NewAmmoFallingDown;
	AmmoImpulseForce = NewAmmoImpulseForce;
}


