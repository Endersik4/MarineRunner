// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MARINERUNNER_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interact")
		void BreakObject(float ImpulseForce, FRotator BulletRotation, const FHitResult& NewHit);

	UFUNCTION()
		virtual void ApplyDamage(float NewDamage, float NewImpulse, FVector ImpulseDirection, const FHitResult& NewHit) = 0;
};
