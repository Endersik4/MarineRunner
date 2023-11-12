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
		int32 IndexInList;
	UPROPERTY(EditAnywhere)
		int32 Floor;
	UPROPERTY(EditAnywhere)
		bool bAccessible = true;
	UPROPERTY(EditAnywhere, meta = (EditConditionHides = "bAccessible"))
		FName NotAccessibleText;
	UPROPERTY(EditAnywhere)
		FVector FloorLocation;

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
		UImage* ElevatorGoesUpDownImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* WaitForElevatorTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* MaintanceTextBlock;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* MaintanceModeTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UListView* SelectFloorsListView;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* SelectFloorsAppearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* SelectFloorsDisappearAnim;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* WaitTextBlockAppearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* WaitTextBlockDisappearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* WaitTextBlockActiveAnim;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* ElevatorGoesUpDownAppearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* ElevatorGoesUpDownDisappearAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		TArray<FElevatorFloor> ElevatorFloors;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float ElevatorGoesUpImageAngle = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float ElevatorGoesDownImageAngle = 180.f;

	FORCEINLINE void SetElevator(class AElevator* NewElevatorActor) { ElevatorActor = NewElevatorActor; }

	UFUNCTION()
		void ActiveSelectFloorPanel(bool NewIsElevatorInMove);

	UFUNCTION(BlueprintCallable)
		void SelectFloor(int32 FloorToGo);

	void ActivateWaitForElevatorText(bool bActivate = true);
	void ActivateElevatorGoesUpDownImage(bool bActivate = true, FVector FloorLocationToGo = FVector(0.f));

private:
	class AElevator* ElevatorActor;
	bool bIsElevatorInMove = false;
};
