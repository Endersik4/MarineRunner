// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/Components/DismemberEnemyComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "DismemberedLimbActor.h"
UDismemberEnemyComponent::UDismemberEnemyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UDismemberEnemyComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UDismemberEnemyComponent::DismemberLimb(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, float ImpulseForce, const FColor& BloodSprayColor, float RadialImpulseRadius)
{
	if (!IsValid(SkeletalMeshToDismember))
		return;

	FDismemberLimb* DismemberedLimb = SpawnLimbActor(SkeletalMeshToDismember, HitBoneResult, ImpulseForce, RadialImpulseRadius);
	if (!DismemberedLimb)
		return;

	SkeletalMeshToDismember->HideBoneByName(HitBoneResult.BoneName, EPhysBodyOp::PBO_MAX);
	AllTerminatedBones.Add(HitBoneResult.BoneName);

	// impulse is added to hit bone, have to terminate bone later for impulse to work
	FTimerHandle TerminateBoneHandle;
	FTimerDelegate TerminateBoneDelegate = FTimerDelegate::CreateUObject(this, &UDismemberEnemyComponent::TerminateBone, SkeletalMeshToDismember, HitBoneResult.BoneName);
	GetWorld()->GetTimerManager().SetTimer(TerminateBoneHandle, TerminateBoneDelegate, TimeToTerminateBone, false);

	SpawnBloodSprayParticle(DismemberedLimb, SkeletalMeshToDismember, BloodSprayColor);

	// dont spawn the same limb again
	AllLimbsToDismember.RemoveSingle(*DismemberedLimb);
}

FDismemberLimb* UDismemberEnemyComponent::SpawnLimbActor(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, const float& ImpulseForce, const float& RadialImpulseRadius)
{
	FDismemberLimb* LimbToDismbember = AllLimbsToDismember.FindByKey(HitBoneResult.BoneName);
	if (!LimbToDismbember)
		return nullptr;

	if (!IsValid(LimbToDismbember->LimbSkeletalMesh))
		return nullptr;

	FRotator LimbRotation = LimbToDismbember->LimbRotation + SkeletalMeshToDismember->GetBoneQuaternion(HitBoneResult.BoneName).Rotator();
	FTransform SpawnedLimbTransform = FTransform(LimbRotation, SkeletalMeshToDismember->GetBoneLocation(HitBoneResult.BoneName), SkeletalMeshToDismember->GetRelativeScale3D());
	TObjectPtr<ADismemberedLimbActor> SpawnedLimb = GetWorld()->SpawnActor<ADismemberedLimbActor>(ADismemberedLimbActor::StaticClass(), SpawnedLimbTransform);
	if (!IsValid(SpawnedLimb))
		return nullptr;

	SpawnedLimb->SetEnemyLimbOwner(GetOwner());

	SpawnedLimb->SetLifeSpan(LimbLifeSpan);
	if (IsValid(GetOwner()))
		GetOwner()->SetLifeSpan(LimbLifeSpan+ AdditionalLifeSpanForEnemyOwner); // the spawned limb has a pointer to EnemyOwner. This prevents the enemy from destroying itself before limb

	SpawnedLimb->GetLimbSkeletalMesh()->SetSkeletalMesh(LimbToDismbember->LimbSkeletalMesh);
	SpawnedLimb->GetLimbSkeletalMesh()->RecreatePhysicsState();

	if (RadialImpulseRadius != 0.f)
	{
		SpawnedLimb->GetLimbSkeletalMesh()->AddRadialImpulse(HitBoneResult.TraceStart, RadialImpulseRadius, ImpulseForce * ImpulseForceMultiplier, ERadialImpulseFalloff::RIF_Linear, true);
	}
	else
	{
		FVector BulletDirection = UKismetMathLibrary::FindLookAtRotation(HitBoneResult.TraceStart, HitBoneResult.TraceEnd).Vector();
		SpawnedLimb->GetLimbSkeletalMesh()->AddImpulse(BulletDirection * ImpulseForce * ImpulseForceMultiplier, HitBoneResult.BoneName, true);
	}

	for (const FName& TerminatedBone : AllTerminatedBones)
	{
		SpawnedLimb->GetLimbSkeletalMesh()->HideBoneByName(TerminatedBone, EPhysBodyOp::PBO_Term);
	}

	return LimbToDismbember;
}

void UDismemberEnemyComponent::TerminateBone(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, FName BoneNameToTerminate)
{
	if (!IsValid(SkeletalMeshToDismember))
		return;

	SkeletalMeshToDismember->HideBoneByName(BoneNameToTerminate, EPhysBodyOp::PBO_Term);
}

void UDismemberEnemyComponent::SpawnBloodSprayParticle(FDismemberLimb* DismemberedLimb, TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FColor& BloodSprayColor)
{
	if (!IsValid(BloodSprayFromLimbNiagaraSystem))
		return;

	const FName& ParentBoneName = SkeletalMeshToDismember->GetParentBone(DismemberedLimb->BoneName);
	FRotator BloodSprayRotation = SkeletalMeshToDismember->GetBoneQuaternion(ParentBoneName).Rotator() + BloodSprayAddRotation;
	TObjectPtr<UNiagaraComponent> SpawnedNiagaraParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(BloodSprayFromLimbNiagaraSystem, SkeletalMeshToDismember, ParentBoneName,
			SkeletalMeshToDismember->GetBoneLocation(ParentBoneName), BloodSprayRotation, EAttachLocation::KeepWorldPosition, true);
	
	if (!IsValid(SpawnedNiagaraParticle))
		return;

	SpawnedNiagaraParticle->SetColorParameter(FName(TEXT("BloodColor")), BloodSprayColor);
}

