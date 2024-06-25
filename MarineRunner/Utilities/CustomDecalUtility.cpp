// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Utilities/CustomDecalUtility.h"

#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

TObjectPtr<class UDecalComponent> UCustomDecalUtility::SpawnDecalAtLocation(const FDecalBase& DecalToSpawn, UWorld* World, const FVector& DecalLocation, const FRotator& DecalRotation, const FVector& DecalSize)
{
	if (!IsValid(DecalToSpawn.DecalMaterial))
		return nullptr;

	TObjectPtr<UDecalComponent> SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(World, DecalToSpawn.DecalMaterial, DecalSize, DecalLocation, DecalRotation);

	if (!IsValid(SpawnedDecal))
		return nullptr;

	SpawnedDecal->SetFadeScreenSize(0.f);
	SpawnedDecal->SetFadeOut(DecalToSpawn.DecalFadeOutDelay, DecalToSpawn.DecalFadeOutDuration, false);

	return SpawnedDecal;
}

TObjectPtr<class UDecalComponent> UCustomDecalUtility::SpawnDecalAttached(const FDecalBase& DecalToSpawn, TObjectPtr<USceneComponent> AttachToComponent, FName BoneNameToAttach, const FVector& DecalLocation, const FRotator& DecalRotation, const FVector& DecalSize, EAttachLocation::Type AttachLocationType)
{
	if (!IsValid(DecalToSpawn.DecalMaterial))
		return nullptr;

	TObjectPtr<UDecalComponent> SpawnedDecal = UGameplayStatics::SpawnDecalAttached(DecalToSpawn.DecalMaterial, DecalSize, AttachToComponent, BoneNameToAttach, DecalLocation, DecalRotation, AttachLocationType);

	if (!IsValid(SpawnedDecal))
		return nullptr;

	SpawnedDecal->SetFadeScreenSize(0.f);
	SpawnedDecal->SetFadeOut(DecalToSpawn.DecalFadeOutDelay, DecalToSpawn.DecalFadeOutDuration, false);

	return SpawnedDecal;
}