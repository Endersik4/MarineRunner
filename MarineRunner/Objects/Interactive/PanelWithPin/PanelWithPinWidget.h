// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanelWithPinWidget.generated.h"

UCLASS()
class MARINERUNNER_API UDoorPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> InteractDoorButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> InteractDoorText;

	// If pin is in use
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> BackgroundPinImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> PinCodeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTileView> PinNumbersTileView;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> OnClickedInteractDoorAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> HoverInteractDoorAnim = nullptr;

	UFUNCTION()
		void OnClickedInteractDoorButton();
	UFUNCTION()
		void OnHoveredInteractDoorButton();
	UFUNCTION()
		void OnUnhoveredInteractDoorButton();

	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings")
		FText OpenDoorText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings")
		FText CloseDoorText;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings")
		float TimeToChangeStatusText = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TObjectPtr < USoundBase> WrongCodeSound;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TArray<int32> PinNumberEntries;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TSubclassOf<class UPinNumberEntryObject> PinNumberEntryObjectClass;

	FORCEINLINE void SetActorToUnlock(class IUsePinToEnterInterface* NewActorWithWidget) { ActorWithWidget = NewActorWithWidget; }
	FORCEINLINE void SetCanCloseObject(bool bCan) { bCanCloseObject = bCan; }

	void AddNumberToEnteredPin(int32 Number);
	void ChangeToUsePin(int32 PinCode);
	void PlayOpenCloseEffects();

	void PinIsCorrect(bool bClickedByOwner = false);
	void RestartDoorPanelWidget();

private:

	bool bDoorOpen = false;
	bool bCanCloseObject = true;

	FTimerHandle ChangeDoorStatusHandle;
	void ChangeDoorStatus();

	class IUsePinToEnterInterface* ActorWithWidget;

	int32 CurrentPinCode;
	FString CurrentlyEnteredPin;
	FString CurrentlyEnteredPin_Text;

};
