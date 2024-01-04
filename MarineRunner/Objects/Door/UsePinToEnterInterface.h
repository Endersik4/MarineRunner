// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UsePinToEnterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUsePinToEnterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MARINERUNNER_API IUsePinToEnterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
		virtual void ClickedOpenButton(class UDoorPanelWidget* ClickedWidget = nullptr) = 0;

	UFUNCTION()
		virtual void PinCorrect() = 0;

	UFUNCTION()
		virtual class UDoorPanelWidget* GetOtherPanelWidget(class UDoorPanelWidget* PanelActivatedByPlayer) = 0;

};
