// Copyright Adam Bartela.All Rights Reserved


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
	virtual void TakeItem(class AMarineCharacter* Character) = 0;

	UFUNCTION()
	virtual void ItemHover(class AMarineCharacter* Character) = 0;

	UFUNCTION()
	virtual void ItemUnHover(class AMarineCharacter* Character) = 0;
};
