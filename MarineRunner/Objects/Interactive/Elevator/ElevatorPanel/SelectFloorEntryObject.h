// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MarineRunner/Objects/Interactive/Elevator/ElevatorPanel/ElevatorFloorData.h"
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
		FElevatorFloor ElevatorFloor = FElevatorFloor();
	UPROPERTY(EditDefaultsOnly, Category = "Select Floors Settings")
		TObjectPtr<class UElevatorPanelWidget> ElevatorPanelWidget = nullptr;

	bool operator==(const USelectFloorEntryObject& OtherElevatorPanel);
	bool operator==(const int32 FloorToGo);
	bool operator!=(const int32 FloorToGo);

};
