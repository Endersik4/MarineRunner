// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TakeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTakeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MARINERUNNER_API ITakeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
		virtual void TakeItem(class AMarineCharacter* Character, bool & bIsItWeapon) = 0;

	UFUNCTION()
		virtual AActor* DropItem() = 0;

	UFUNCTION()
		virtual bool ItemLocationWhenGrabbed(float SpeedOfItem) = 0;

	UFUNCTION()
		virtual void ItemHover(class UHUDWidget* MarineHUDWidget) = 0;
	
	UFUNCTION()
		virtual void ItemUnHover(class UHUDWidget* MarineHUDWidget) = 0;
};
