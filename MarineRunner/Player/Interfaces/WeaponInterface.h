// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MARINERUNNER_API IWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void TakeWeapon(class AMarineCharacter* Player, bool bWasOnceTaken, int32 CurrentMagazineCapacityToLoad) = 0;

	UFUNCTION()
	virtual void DrawWeapon() = 0;
	UFUNCTION()
	virtual void PutAwayWeapon() = 0;
	UFUNCTION()
	virtual void HideWeapon() = 0;
	UFUNCTION()
	virtual void SetDropWeapon(bool bDrop) = 0;

	UFUNCTION()
	virtual void PrimaryAction() = 0;
	UFUNCTION()
	virtual void ReleasedPrimaryAction() = 0;

	UFUNCTION()
	virtual void SecondaryAction() = 0;
	UFUNCTION()
	virtual void ReleasedSecondaryAction() = 0;

	UFUNCTION()
	virtual void TertiaryAction(float Value) = 0;

	UFUNCTION()
	virtual void ActionFromKey_One() = 0;

	UFUNCTION()
	virtual void UpdateWeaponHudInformation() = 0;

	UFUNCTION()
	virtual FString GetPathToWeaponClass() = 0;

	UFUNCTION()
	virtual int32 GetIntValueToSave() = 0;
};
