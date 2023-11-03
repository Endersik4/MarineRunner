// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MARINERUNNER_API IEnemyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
		virtual class USkeletalMeshComponent* GetSkeletalMesh() = 0;

	UFUNCTION()
		virtual const FVector GetPlayerCameraLocation() = 0;
	UFUNCTION()
		virtual void PlayShootAnimation() = 0;
	UFUNCTION()
		virtual void AddImpulseToPhysicsMesh(const FVector & Impulse) = 0;
	UFUNCTION()
		virtual const bool GetIsDead() = 0;
};
