// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteActor.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"

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

void ABullet::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BulletHits)
	{
		HitParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletHits, Hit.ImpactPoint);
	}

	if (OtherActor->ActorHasTag("Enemy"))
	{
		AEnemyPawn* Enemy = Cast<AEnemyPawn>(OtherActor);
		if (Enemy)
		{
			FColor BloodColor = Enemy->GetBloodColor();
			//I cant just do SetColorParameter(TEXT("BloodColor"), Enemy->GetBloodColor()); because of some bug it doesnt work, even when 
			//Enemy->GetBloodColor() returns FLinearColor. It just dont work
			HitParticle->SetColorParameter(TEXT("BloodColor"), FLinearColor(BloodColor.R, BloodColor.G, BloodColor.B));
			Enemy->SetHealth(Enemy->GetHealth() - Damage);
			if (Enemy->GetHealth() <= 0.f)
			{
				Enemy->GetEnemySkeletalMesh()->SetSimulatePhysics(true);
				Enemy->SetIsDead(true);

				FVector Impulse = GetActorForwardVector() * (AmmoImpulseForce * 100);
				Enemy->GetEnemySkeletalMesh()->AddImpulse(Impulse, Hit.BoneName);

				Enemy->GetEnemySkeletalMesh()->Stop();
				Enemy->GetEnemySkeletalMesh()->Stop();
			}
			
		}
	}
	else if (HitParticle)
	{
		HitParticle->SetColorParameter(TEXT("BloodColor"), FLinearColor::Yellow);

		FVector Location = Hit.ImpactPoint + (-GetActorForwardVector() * 3.f);
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += 90.f;
		//w build.cs trzeba wpisać Paper2D
		//#include PaperSpriteActor.h | #include PaperSprite.h. 
		//Respienie PaperSpriteActor w swiecie
		APaperSpriteActor* BulletHole = GetWorld()->SpawnActor<APaperSpriteActor>(APaperSpriteActor::StaticClass(), 
			Location, Rotation);
		if (BulletHole)
		{
			//BulletHoleSprite to UPaperSprite* w .h
			if (BulletHoleSprite)
			{
				//Trzeba wlaczyc aby zmienic Sprite actora
				BulletHole->GetRenderComponent()->SetMobility(EComponentMobility::Movable); 
				//Zmiana Spritu (GetRenderComponent() to wskaznik na Sprite)
				BulletHole->GetRenderComponent()->SetSprite(BulletHoleSprite); 
			}
			BulletHole->SetActorScale3D(FVector(0.01));
		}
	}

	Destroy();
}


