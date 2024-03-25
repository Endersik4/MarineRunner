// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElevatorFloorData.h"
#include "ElevatorPanelWidget.generated.h"


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
		TObjectPtr<UImage> BackgroundImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> ElevatorGoesUpDownImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> WaitForElevatorTextBlock = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> MaintanceTextBlock = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> MaintanceModeTextBlock = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UListView> SelectFloorsListView = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SelectFloorsAppearAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SelectFloorsDisappearAnim = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WaitTextBlockAppearAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WaitTextBlockDisappearAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WaitTextBlockActiveAnim = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ElevatorGoesUpDownAppearAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ElevatorGoesUpDownDisappearAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		TSubclassOf<class USelectFloorEntryObject> SelectedFloorEntryObject = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float ElevatorGoesUpImageAngle = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float ElevatorGoesDownImageAngle = 180.f;

	FORCEINLINE void SetElevator(TObjectPtr<class AElevator> NewElevatorActor) { ElevatorActor = NewElevatorActor; }
	FORCEINLINE void SetElevatorFloors(TArray<FElevatorFloor> & NewFloors) { AllElevatorFloors = NewFloors;}
	bool GetDoorOpen() const;
	int32 GetCurrentFloor() const;

	UFUNCTION()
		void ShowSelectFloorPanel(bool NewIsElevatorInMove);

	void SelectFloor(int32 FloorToGo);

	void ShowWaitForElevatorText(bool bActivate = true);
	void ShowElevatorGoesUpDownImage(bool bActivate = true, FVector FloorLocationToGo = FVector(0.f));

	void FillSelectFloorsListView();
private:

	UPROPERTY(Transient)
		TArray<FElevatorFloor> AllElevatorFloors = {FElevatorFloor()};

	UPROPERTY(Transient)
		TObjectPtr<class AElevator> ElevatorActor = nullptr;
};
