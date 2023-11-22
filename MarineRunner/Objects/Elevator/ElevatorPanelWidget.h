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
		TSubclassOf<class USelectFloorEntryObject> SelectedFloorEntryObject;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float ElevatorGoesUpImageAngle = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float ElevatorGoesDownImageAngle = 180.f;

	FORCEINLINE void SetElevator(class AElevator* NewElevatorActor) { ElevatorActor = NewElevatorActor; }
	int32 GetCurrentFloor() const;

	UFUNCTION()
		void ActiveSelectFloorPanel(bool NewIsElevatorInMove);

	void SelectFloor(int32 FloorToGo);

	void ActivateWaitForElevatorText(bool bActivate = true);
	void ActivateElevatorGoesUpDownImage(bool bActivate = true, FVector FloorLocationToGo = FVector(0.f));

	void FillSelectFloorsListView();

private:
	class AElevator* ElevatorActor;

};
