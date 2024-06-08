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
	TObjectPtr<UImage> BackgroundImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> CallElevatorButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> CallElevatorText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> WaitForElevatorText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> MaintanceTextBlock = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> MaintanceModeTextBlock = nullptr;

	// If pin is in use
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> BackgroundPinImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PinCodeText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTileView> PinNumbersTileView = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> HoverCallElevatorAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> AppearCallElevatorAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> DisappearCallElevatorAnim = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> AppearWaitForElevatorAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> DisappearWaitForElevatorAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> ActiveWaitForElevatorAnim = nullptr;

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
	TArray<int32> PinNumberEntries = { 0 };
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
	TSubclassOf<class UPinNumberEntryObject> PinNumberEntryObjectClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
	TObjectPtr<USoundBase> WrongCodeSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
	FText LockedByPinModeText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
	FText UnlockedByPinModeText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
	FSlateColor LockedByPinColorText = FSlateColor();;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
	FSlateColor UnlockedByPinColorText = FSlateColor();

	UPROPERTY(Transient)
	int32 FloorToMove = 0;

	UPROPERTY(Transient)
	TObjectPtr<class AElevator> ElevatorActor = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AOutsideElevatorDoor> OutsideElevatorActor = nullptr;

	void ShowWaitText(bool bShow = true);
	void ShowCallElevatorPanel(bool bShow = true);
	void HidePin(bool bHide);

	UPROPERTY(Transient)
	int32 CurrentPinCode = 0;
	UPROPERTY(Transient)
	FString CurrentlyEnteredPin = FString("");
	UPROPERTY(Transient)
	FString CurrentlyEnteredPin_Text = FString("");
};
