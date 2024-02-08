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

	// If pin is in use
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundPinImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* PinCodeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* PinNumbersTileView;

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
	FORCEINLINE void SetOutsideElevatorDoor(class AOutsideElevatorDoor* NewOutsideElevatorActor) { OutsideElevatorActor = NewOutsideElevatorActor; }

	bool CanCallElevator() const;

	void CallElevatorAction(const enum ECallElevatorAction ActionToDo);

	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		USoundBase* WrongCodeSound;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TArray<int32> PinNumberEntries;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TSubclassOf<class UPinNumberEntryObject> PinNumberEntryObjectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FText LockedByPinModeText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FText UnlockedByPinModeText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FSlateColor LockedByPinColorText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin|Effects")
		FSlateColor UnlockedByPinColorText;

	void AddNumberToEnteredPin(int32 Number);
	void ChangeDoorPanelToUsePin(int32 PinCode);

	void PinIsCorrect();

private:
	int32 Floor;
	class AElevator* ElevatorActor;
	class AOutsideElevatorDoor* OutsideElevatorActor;

	void ShowWaitText(bool bShow = true);
	void ShowCallElevatorPanel(bool bShow = true);

	int32 CurrentPinCode;
	FString CurrentlyEnteredPin;
	FString CurrentlyEnteredPin_Text;

};
