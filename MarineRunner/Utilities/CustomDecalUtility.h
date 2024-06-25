// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MarineRunner/Interfaces/DamageInterface.h"

#include "CustomDecalUtility.generated.h"

USTRUCT(BlueprintType)
struct FDecalBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	FVector DecalSize = FVector();
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> DecalMaterial = nullptr;
	UPROPERTY(EditAnywhere)
	float DecalFadeOutDelay = 0.f;
	UPROPERTY(EditAnywhere)
	float DecalFadeOutDuration = 0.f;

	FDecalBase()
	{
		DecalSize = FVector();
		DecalMaterial = nullptr;
		DecalFadeOutDelay = 0.f;
		DecalFadeOutDuration = 0.f;
	}

	FDecalBase(FVector _DecalSize, TObjectPtr<UMaterialInstance> _DecalMaterial, float _DecalFadeOutDelay, float _DecalFadeOutDuration)
	{
		DecalSize = _DecalSize;
		DecalMaterial = _DecalMaterial;
		DecalFadeOutDelay = _DecalFadeOutDelay;
		DecalFadeOutDuration = _DecalFadeOutDuration;
	}
};

USTRUCT(BlueprintType)
struct FWoundDecal : public FDecalBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EWeaponType> WoundFromWeaponType;
	UPROPERTY(EditAnywhere)
	FFloatRange WoundRandomSizeMultiplierRange = FFloatRange();

	const bool operator==(const EWeaponType& otherWeaponType) const
	{
		return WoundFromWeaponType == otherWeaponType;
	}

	FWoundDecal() : FDecalBase()
	{
		WoundFromWeaponType = EWeaponType::EWT_Gun;
		WoundRandomSizeMultiplierRange = FFloatRange();
	}

	FWoundDecal(TEnumAsByte<EWeaponType> _WoundFromWeaponType, FFloatRange _WoundRandomSizeMultiplierRange) : FDecalBase(DecalSize, DecalMaterial, DecalFadeOutDelay, DecalFadeOutDuration)
	{
		WoundFromWeaponType = _WoundFromWeaponType;
		WoundRandomSizeMultiplierRange = _WoundRandomSizeMultiplierRange;
	}
};

UCLASS()
class MARINERUNNER_API UCustomDecalUtility : public UObject
{
	GENERATED_BODY()

public:
	static TObjectPtr<class UDecalComponent> SpawnDecalAtLocation(const FDecalBase& DecalToSpawn, class UWorld* World, const FVector& DecalLocation, const FRotator& DecalRotation, const FVector& DecalSize);

	static TObjectPtr<class UDecalComponent> SpawnDecalAttached(const FDecalBase& DecalToSpawn, TObjectPtr<USceneComponent> AttachToComponent, FName BoneNameToAttach, const FVector& DecalLocation, const FRotator& DecalRotation, const FVector& DecalSize, EAttachLocation::Type AttachLocationType = EAttachLocation::KeepWorldPosition);
};
