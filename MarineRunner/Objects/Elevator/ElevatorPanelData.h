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
		FSlateColor NotAccessibleTextColor = FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f));;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bAccessible", EditConditionHides))
		FLinearColor NotAccessibleButtonColor = FLinearColor(1.f, 0.f, 0.f, 1.f);;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAccessible", EditConditionHides))
		FVector FloorLocation;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAccessible", EditConditionHides))
		FSlateColor AccessibleTextColor = FSlateColor(FLinearColor(0.f, 0.9f, 1.f, 1.f));;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAccessible", EditConditionHides))
		FLinearColor AccessibleButtonColor = FLinearColor(0.68f, 0.045f, 0.8f, 1.f);;

public:
	FElevatorFloor()
	{
		Floor = 0;
		bAccessible = true;
		NotAccessibleText = FText();
		NotAccessibleTextColor = FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f));
		NotAccessibleButtonColor = FLinearColor(1.f, 0.f, 0.f, 1.f);
		FloorLocation = FVector(0.f);
		AccessibleTextColor = FSlateColor(FLinearColor(0.f, 0.9f, 1.f, 1.f));
		AccessibleButtonColor = FLinearColor(0.68f,0.045f, 0.8f,1.f);
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
