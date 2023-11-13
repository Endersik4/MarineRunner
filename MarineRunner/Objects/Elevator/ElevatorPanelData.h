// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ElevatorPanelData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FElevatorFloor
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		int32 Floor;
	// This is the floor where the elevator starts
	UPROPERTY(EditAnywhere)
		bool bStartingFloor = false;
	UPROPERTY(EditAnywhere)
		bool bAccessible = true;
	UPROPERTY(EditAnywhere, meta = (EditConditionHides = "bAccessible"))
		FText NotAccessibleText;
	UPROPERTY(EditAnywhere)
		FVector FloorLocation;

public:
	FElevatorFloor()
	{
		Floor = 0;
		bAccessible = true;
		FloorLocation = FVector(0.f);
	}

	FElevatorFloor(int32 _Floor, bool _bAccessible, FVector _FloorLocation)
	{
		Floor = _Floor;
		bAccessible = _bAccessible;
		FloorLocation = _FloorLocation;
	}

	bool operator==(const int32& OtherFloor)
	{
		return Floor == OtherFloor;
	}
};

class MARINERUNNER_API ElevatorPanelData
{
public:
	
};
