// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElevatorPanelWidget.generated.h"

USTRUCT(BlueprintType)
struct FElevatorFloor
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		int32 Floor;
	UPROPERTY(EditAnywhere)
		FVector FloorLocation;

	FElevatorFloor()
	{
		Floor = 0;
		FloorLocation = FVector(0.f);
	}

	FElevatorFloor(int32 _Floor, FVector _FloorLocation)
	{
		Floor = _Floor;
		FloorLocation = _FloorLocation;
	}

	bool operator==(const int32& OtherFloor)
	{
		return Floor == OtherFloor;
	}
};
class UTextBlock;
class UImage;
UCLASS()
class MARINERUNNER_API UElevatorPanelWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* ElevatorGoesUpImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* ElevatorGoesDownImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* WaitForElevatorImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* MaintanceTextBlock;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* MaintanceModeTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UVerticalBox* SelectFloorsHorizontalBox;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		TArray<FElevatorFloor> ElevatorFloors;

	FORCEINLINE void SetElevator(class AElevator* NewElevatorActor) { ElevatorActor = NewElevatorActor; }

	UFUNCTION()
		void SetIsElevatorInMove(bool NewIsElevatorInMove);

	UFUNCTION(BlueprintCallable)
		void SelectFloor(int32 FloorToGo);

	void UnhideWaitForElevatorImage();
	void UnhideCorrectElevatorGoesUpImage(FVector FloorLocationToGo);

private:
	class AElevator* ElevatorActor;
	bool bIsElevatorInMove = false;
};
