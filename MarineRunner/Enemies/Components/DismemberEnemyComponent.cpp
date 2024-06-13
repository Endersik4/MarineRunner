// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/Components/DismemberEnemyComponent.h"
#include "Animation/SkeletalMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

	FDismemberLimb* DismemberedLimb = SpawnLimb(SkeletalMeshToDismember, HitBoneResult, ImpulseForce, RadialImpulseRadius);
	if (!DismemberedLimb)
		return;

	SkeletalMeshToDismember->HideBoneByName(HitBoneResult.BoneName, EPhysBodyOp::PBO_MAX);

	// impulse is added to hit bone, have to terminate bone later for impulse to work
	FTimerHandle TerminateBoneHandle;
	FTimerDelegate TerminateBoneDelegate = FTimerDelegate::CreateUObject(this, &UDismemberEnemyComponent::TerminateBone, SkeletalMeshToDismember, HitBoneResult.BoneName);
	GetWorld()->GetTimerManager().SetTimer(TerminateBoneHandle, TerminateBoneDelegate, TimeToTerminateBone, false);

	SpawnBloodSprayParticle(DismemberedLimb, SkeletalMeshToDismember, BloodSprayColor);

	// dont spawn the same limb again
	AllLimbsToDismember.RemoveSingle(*DismemberedLimb);
}

FDismemberLimb* UDismemberEnemyComponent::SpawnLimb(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, const float& ImpulseForce, const float& RadialImpulseRadius)
{
	FDismemberLimb* LimbToDismbember = AllLimbsToDismember.FindByKey(HitBoneResult.BoneName);
	if (!LimbToDismbember)
		return nullptr;

	if (!IsValid(LimbToDismbember->LimbSkeletalMesh))
		return nullptr;

	FRotator LimbRotation = LimbToDismbember->LimbRotation + SkeletalMeshToDismember->GetBoneQuaternion(HitBoneResult.BoneName).Rotator();
	FTransform SpawnedLimbTransform = FTransform(LimbRotation, SkeletalMeshToDismember->GetBoneLocation(HitBoneResult.BoneName), SkeletalMeshToDismember->GetRelativeScale3D());
	TObjectPtr<ASkeletalMeshActor> SpawnedLimb = GetWorld()->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), SpawnedLimbTransform);
	if (!IsValid(SpawnedLimb))
		return nullptr;

	SpawnedLimb->SetLifeSpan(LimbLifeSpan);

	SpawnedLimb->GetSkeletalMeshComponent()->SetSkeletalMesh(LimbToDismbember->LimbSkeletalMesh);
	SpawnedLimb->GetSkeletalMeshComponent()->SetMassScale(NAME_None, LimbToDismbember->LimbMassScale);
	SpawnedLimb->GetSkeletalMeshComponent()->RecreatePhysicsState();

	SpawnedLimb->GetSkeletalMeshComponent()->SetSimulatePhysics(true);

	SpawnedLimb->GetSkeletalMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECR_Ignore); // trace == in air

	if (RadialImpulseRadius != 0.f)
	{
		SpawnedLimb->GetSkeletalMeshComponent()->AddRadialImpulse(HitBoneResult.TraceStart, RadialImpulseRadius, ImpulseForce * ImpulseForceMultiplier, ERadialImpulseFalloff::RIF_Linear, true);
	}
	else
	{
		FVector BulletDirection = UKismetMathLibrary::FindLookAtRotation(HitBoneResult.TraceStart, HitBoneResult.TraceEnd).Vector();
		SpawnedLimb->GetSkeletalMeshComponent()->AddImpulse(BulletDirection * ImpulseForce * ImpulseForceMultiplier, HitBoneResult.BoneName, true);
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
	if (!IsValid(BloodSprayNiagaraSystem))
		return;

	FRotator BloodSprayRotation = SkeletalMeshToDismember->GetBoneQuaternion(DismemberedLimb->UpperBoneName).Rotator() + BloodSprayAddRotation;
	TObjectPtr<UNiagaraComponent> SpawnedNiagaraParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(BloodSprayNiagaraSystem, SkeletalMeshToDismember, DismemberedLimb->UpperBoneName,
			SkeletalMeshToDismember->GetBoneLocation(DismemberedLimb->UpperBoneName), BloodSprayRotation, EAttachLocation::KeepWorldPosition, true);

	if (!IsValid(SpawnedNiagaraParticle))
		return;

	SpawnedNiagaraParticle->SetColorParameter(FName(TEXT("BloodColor")), BloodSprayColor);
}

