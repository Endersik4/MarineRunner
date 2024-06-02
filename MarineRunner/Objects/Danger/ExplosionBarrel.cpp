// Copyright Adam Bartela.All Rights Reserved

#include "ExplosionBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"

#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

AExplosionBarrel::AExplosionBarrel()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ExplosionBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosion Barrel Mesh"));
	RootComponent = ExplosionBarrelMesh;
}

void AExplosionBarrel::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (bExploded) 
		return;

	bExploded = true;

	FTimerHandle SpawnExplosionBarrelGeometryHandle;
	const float SpawnExplosionBarrelAfterTime = 0.02f; // There is a bug when the bullet has physics turned on then Spawning Expolsion Barrel frezzes the game and timer solves it
	GetWorld()->GetTimerManager().SetTimer(SpawnExplosionBarrelGeometryHandle, this, &AExplosionBarrel::SpawnExplosionBarrelGeometry, SpawnExplosionBarrelAfterTime, false);

	FTimerHandle ExplodeHandle;
	GetWorld()->GetTimerManager().SetTimer(ExplodeHandle, this, &AExplosionBarrel::Explode, StartExplodeTime, false);
}

// Called when the game starts or when spawned
void AExplosionBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void AExplosionBarrel::Explode()
{
	TArray<FHitResult> HitArray;
	GetWorld()->SweepMultiByChannel(HitArray, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(ExplosionRadius));

	#ifdef WITH_EDITOR
		if (bDrawDebugRadialSphere)
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 50, FColor::Red, true);
	#endif //!WITH_EDITOR

	//Use UseDamageInterfaceOnActor(HitResult) only once on the same actor
	TArray<TObjectPtr<AActor>> ActorsToApplyDamage;
	for (int i = 0; i != HitArray.Num(); i++)
	{
		ActorsToApplyDamage.AddUnique(HitArray[i].GetActor());
	}

	//Use interface on every actors that was hit by SweepMultiByChannel
	for (const FHitResult& HitResult : HitArray)
	{
		if (ActorsToApplyDamage.Find(HitResult.GetActor()) == INDEX_NONE)
			continue;

		UseDamageInterfaceOnActor(HitResult);
		ActorsToApplyDamage.Remove(HitResult.GetActor());
	}

	SpawnEffects();

	BarrelExplodedSaveData();
	DisableBarrel();
}

void AExplosionBarrel::UseDamageInterfaceOnActor(const FHitResult& HitResult)
{
	IDamageInterface* Interface = Cast<IDamageInterface>(HitResult.GetActor());
	if (Interface) //Check if Object has Interface C++ Implementation
	{
		Interface->ApplyDamage(ExplosionDamage, ExplosionImpulseForce, HitResult, this, ExplosionRadius);
	}
	else if (HitResult.GetActor()->Implements<UDamageInterface>())  //Check if Object has Interface Blueprint Implementation
	{
		IDamageInterface::Execute_BreakObject(HitResult.GetActor(), ExplosionImpulseForce, HitResult, this, ExplosionRadius);
	}
	else if (HitResult.GetComponent()->IsSimulatingPhysics())
	{
		HitResult.GetComponent()->AddRadialImpulse(GetActorLocation(), ExplosionRadius, ExplosionImpulseForce, ERadialImpulseFalloff::RIF_Linear);
	}
}

void AExplosionBarrel::SpawnEffects()
{
	if (IsValid(ExplosionSound))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("Explosion Sound is nullptr in ExplosionBarrel!"));

	if (ExplosionParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), FRotator(0.f), FVector(ExplosionParticleSize));
	else
		UE_LOG(LogTemp, Warning, TEXT("Explosion Particle is nullptr in ExplosionBarrel!"));

	const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(Player))
		return;

	const float& DistanceToPlayer = FVector::Distance(Player->GetActorLocation(), GetActorLocation());
	if (DistanceToPlayer < MaxDistanceToStartShake && DistanceToPlayer != 0)
	{
		float CameraShakeScale = (MaxDistanceToStartShake / DistanceToPlayer) * CameraShakeScaleMultiplier;
		CameraShakeScale = CameraShakeScale > MaxCameraShakeScale ? MaxCameraShakeScale : CameraShakeScale;

		const TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PlayerController))
		{
			PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeAfterExplosion, CameraShakeScale);
		}
	}

	if (!IsValid(ExplosionDecal))
	{
		UE_LOG(LogTemp, Warning, TEXT("Explosion Decal is nullptr in ExplosionBarrel!"));
		return;
	}

	FRotator DecalRotation = ExplosionBarrelMesh->GetComponentRotation();
	DecalRotation.Pitch -= 90.f;
	TObjectPtr<UDecalComponent> SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ExplosionDecal, DecalRotation.Vector() * ExplosionDecalSize, ExplosionBarrelMesh->GetComponentLocation(), DecalRotation);
	if (!IsValid(SpawnedDecal))
		return;
	//SpawnedDecal->SetRelativeScale3D(DecalRotation.Vector().Normalize() * ExplosionDecalSize);
	SpawnedDecal->SetFadeScreenSize(0.f);
}

void AExplosionBarrel::SpawnExplosionBarrelGeometry()
{
	if (!IsValid(ExplosionBarrelGeometryClass))
		return;

	SpawnedBarrelGeometry = GetWorld()->SpawnActor<AActor>(ExplosionBarrelGeometryClass, GetActorLocation(), GetActorRotation());
}

void AExplosionBarrel::BarrelExplodedSaveData()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void AExplosionBarrel::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1)
	{
		DisableBarrel();
	}
	else
	{
		SetActorLocation(SavedCustomData.ObjectTransform.GetLocation());
		SetActorRotation(SavedCustomData.ObjectTransform.GetRotation());
	}
}

void AExplosionBarrel::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	SavedDataObject->RemoveCustomSaveData(IDkey);
	FCustomDataSaved UpdatedData = SavedCustomData;
	UpdatedData.ObjectTransform = FTransform(GetActorRotation(), GetActorLocation());
	SavedDataObject->AddCustomSaveData(IDkey, UpdatedData);
}

void AExplosionBarrel::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (SavedCustomData.ObjectState == 1)
	{
		DisableBarrel(false);

		try
		{
			if (IsValid(SpawnedBarrelGeometry))
				SpawnedBarrelGeometry->Destroy();
		}
		catch(...){}
	}
	else
	{
		SetActorLocation(SavedCustomData.ObjectTransform.GetLocation());
		SetActorRotation(SavedCustomData.ObjectTransform.GetRotation());
	}
}

void AExplosionBarrel::DisableBarrel(bool bDisable)
{
	bExploded = bDisable;
	SetActorTickEnabled(!bDisable);
	SetActorHiddenInGame(bDisable);
	SetActorEnableCollision(!bDisable);
	ExplosionBarrelMesh->SetSimulatePhysics(!bDisable);
}