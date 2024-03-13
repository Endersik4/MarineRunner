// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CallElevatorPanelWidget.generated.h"

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
		TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> CallElevatorButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> CallElevatorText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> WaitForElevatorText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> MaintanceTextBlock;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> MaintanceModeTextBlock;

	// If pin is in use
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> BackgroundPinImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> PinCodeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTileView> PinNumbersTileView;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> HoverCallElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> AppearCallElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> DisappearCallElevatorAnim;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> AppearWaitForElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> DisappearWaitForElevatorAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> ActiveWaitForElevatorAnim;

	UFUNCTION()
		void OnClickedCallElevatorButton();
	UFUNCTION()
		void OnHoveredCallElevatorButton();
	UFUNCTION()
		void OnUnhoveredCallElevatorButton();

	FORCEINLINE void SetElevator(TObjectPtr<class AElevator> NewElevatorActor) { ElevatorActor = NewElevatorActor; }
	FORCEINLINE void SetFloor(int32 NewFloor) { FloorToMove = NewFloor; }
	FORCEINLINE void SetOutsideElevatorDoor(TObjectPtr<class AOutsideElevatorDoor> NewOutsideElevatorActor) { OutsideElevatorActor = NewOutsideElevatorActor; }

	bool CanCallElevator() const;

	void CallElevatorAction(const enum ECallElevatorAction ActionToDo);

	void AddNumberToEnteredPin(int32 Number);
	void ChangeToUsePin(int32 PinCode);

	void PinIsCorrect();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		int32 MaxPinCodeLength = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TArray<int32> PinNumberEntries;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TSubclassOf<class UPinNumberEntryObject> PinNumberEntryObjectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TObjectPtr<USoundBase> WrongCodeSound;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FText LockedByPinModeText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FText UnlockedByPinModeText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FSlateColor LockedByPinColorText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FSlateColor UnlockedByPinColorText;

	int32 FloorToMove;
	UPROPERTY(Transient)
		TObjectPtr<class AElevator> ElevatorActor;
	UPROPERTY(Transient)
		TObjectPtr<class AOutsideElevatorDoor> OutsideElevatorActor;

	void ShowWaitText(bool bShow = true);
	void ShowCallElevatorPanel(bool bShow = true);
	void HidePin(bool bHide);

	int32 CurrentPinCode;
	FString CurrentlyEnteredPin;
	FString CurrentlyEnteredPin_Text;
};
