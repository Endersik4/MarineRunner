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

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAccessible", EditConditionHides))
		int32 Floor;
	UPROPERTY(EditAnywhere)
		bool bAccessible = true;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bAccessible", EditConditionHides))
		FText NotAccessibleText;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bAccessible", EditConditionHides))
		FSlateColor NotAccessibleTextColor;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bAccessible", EditConditionHides))
		FLinearColor NotAccessibleButtonColor;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAccessible", EditConditionHides))
		FVector FloorLocation;

public:
	FElevatorFloor()
	{
		Floor = 0;
		bAccessible = true;
		NotAccessibleText = FText();
		NotAccessibleTextColor = FSlateColor();
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
