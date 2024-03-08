// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelData.h"
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
		TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> ElevatorGoesUpDownImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> WaitForElevatorTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> MaintanceTextBlock;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> MaintanceModeTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UListView> SelectFloorsListView;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SelectFloorsAppearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SelectFloorsDisappearAnim;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WaitTextBlockAppearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WaitTextBlockDisappearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WaitTextBlockActiveAnim;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ElevatorGoesUpDownAppearAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ElevatorGoesUpDownDisappearAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		TSubclassOf<class USelectFloorEntryObject> SelectedFloorEntryObject;
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
	TArray<FElevatorFloor> AllElevatorFloors;
	TObjectPtr<class AElevator> ElevatorActor;
};
