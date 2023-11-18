// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelData.h"
#include "SelectFloorEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API USelectFloorEntryObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Select Floors Settings")
		FElevatorFloor ElevatorFloor;
	UPROPERTY(EditDefaultsOnly, Category = "Select Floors Settings")
		class UElevatorPanelWidget* ElevatorPanelWidget;

	bool operator==(const USelectFloorEntryObject& OtherElevatorPanel);
	bool operator==(const int32 FloorToGo);
	bool operator!=(const int32 FloorToGo);

};
