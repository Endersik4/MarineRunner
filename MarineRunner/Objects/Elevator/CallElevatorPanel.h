// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CallElevatorPanel.generated.h"

class UImage;
class UTextBlock;
UCLASS()
class MARINERUNNER_API UCallElevatorPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* CallElevatorButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* CallElevatorText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* WaitForElevatorText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* MaintanceTextBlock;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* MaintanceModeTextBlock;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* HoverCallElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* AppearCallElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* DisappearCallElevatorAnim;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* AppearWaitForElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* DisappearWaitForElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* ActiveWaitForElevatorAnim;

	UFUNCTION()
		void OnClickedCallElevatorButton();
	UFUNCTION()
		void OnHoveredCallElevatorButton();
	UFUNCTION()
		void OnUnhoveredCallElevatorButton();

	FORCEINLINE void SetElevator(class AElevator* NewElevatorActor) { ElevatorActor = NewElevatorActor; }
	FORCEINLINE void SetFloor(int32 NewFloor) { Floor = NewFloor; }

	bool CanCallElevator() const;

	void CallElevatorAction(const enum ECallElevatorAction ActionToDo);

private:
	int32 Floor;
	class AElevator* ElevatorActor;

	void ShowWaitText(bool bShow = true);
	void ShowCallElevatorPanel(bool bShow = true);
};
