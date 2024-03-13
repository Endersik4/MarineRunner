// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PinNumberEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UPinNumberEntryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Select Floors Settings")
		int32 PinNumber;
	UPROPERTY(EditDefaultsOnly, Category = "Select Floors Settings")
		TObjectPtr<class UDoorPanelWidget> DoorPanelWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Select Floors Settings")
		TObjectPtr<class UCallElevatorPanel> CallElevatorPanelWidget;
	

};
