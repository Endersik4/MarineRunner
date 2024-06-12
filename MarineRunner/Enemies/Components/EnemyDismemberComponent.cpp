// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/Components/EnemyDismemberComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"

UEnemyDismemberComponent::UEnemyDismemberComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UEnemyDismemberComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UEnemyDismemberComponent::DismemberLimb(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, float ImpulseForce, float RadialImpulseRadius)
{
	if (!IsValid(SkeletalMeshToDismember))
		return;

	FDismemberLimb* LimbToDismbember = AllLimbsToDismember.FindByKey(HitBoneResult.BoneName);
	if (!LimbToDismbember)
		return;

	if (!IsValid(LimbToDismbember->LimbMesh))
		return;

	FTransform SpawnedLimbTransform = FTransform(SkeletalMeshToDismember->GetBoneQuaternion(HitBoneResult.BoneName), SkeletalMeshToDismember->GetBoneLocation(HitBoneResult.BoneName));
	TObjectPtr<AStaticMeshActor> SpawnedLimb = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnedLimbTransform);
	if (!IsValid(SpawnedLimb))
		return;

	SpawnedLimb->SetMobility(EComponentMobility::Movable);
	SpawnedLimb->AddActorWorldRotation(FRotator(-90.f, 0.f, 0.f));
	SpawnedLimb->SetLifeSpan(30.f);

	SpawnedLimb->GetStaticMeshComponent()->SetSimulatePhysics(true);
	SpawnedLimb->GetStaticMeshComponent()->SetMassScale(NAME_None, 10.f);
	SpawnedLimb->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Ignore);
	SpawnedLimb->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	SpawnedLimb->GetStaticMeshComponent()->SetStaticMesh(LimbToDismbember->LimbMesh);

	if (RadialImpulseRadius != 0.f)
		SpawnedLimb->GetStaticMeshComponent()->AddRadialImpulse(HitBoneResult.TraceStart, RadialImpulseRadius, 0.f, ERadialImpulseFalloff::RIF_Linear, true);
	else
	{
		FRotator Test = UKismetMathLibrary::FindLookAtRotation(HitBoneResult.TraceStart, HitBoneResult.TraceEnd);
		SpawnedLimb->GetStaticMeshComponent()->AddImpulse(Test.Vector() * ImpulseForce, NAME_None, true);
	}

	AllLimbsToDismember.RemoveSingle(*LimbToDismbember);

	SkeletalMeshToDismember->HideBoneByName(HitBoneResult.BoneName, EPhysBodyOp::PBO_MAX);

	FTimerHandle TerminateBoneHandle;
	FTimerDelegate TerminateBoneDelegate = FTimerDelegate::CreateUObject(this, &UEnemyDismemberComponent::TerminateBone, SkeletalMeshToDismember, HitBoneResult.BoneName);
	GetWorld()->GetTimerManager().SetTimer(TerminateBoneHandle, TerminateBoneDelegate, 0.05f, false);
}

void UEnemyDismemberComponent::TerminateBone(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, FName BoneNameToTerminate)
{
	if (!IsValid(SkeletalMeshToDismember))
		return;

	SkeletalMeshToDismember->HideBoneByName(BoneNameToTerminate, EPhysBodyOp::PBO_Term);
}
