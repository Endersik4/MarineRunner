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

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> InteractDoorButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> InteractDoorText = nullptr;

	// If pin is in use
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> BackgroundPinImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PinCodeText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTileView> PinNumbersTileView = nullptr;

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
	FText OpenDoorText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings")
	FText CloseDoorText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings")
	float TimeToChangeStatusText = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
	TObjectPtr < USoundBase> WrongCodeSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
	TArray<int32> PinNumberEntries = { 0 };
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
	TSubclassOf<class UPinNumberEntryObject> PinNumberEntryObjectClass = nullptr;

public:
	FORCEINLINE void SetActorToUnlock(class IUsePinToEnterInterface* NewActorWithWidget) { ActorWithWidget = NewActorWithWidget; }
	FORCEINLINE void SetCanCloseObject(bool bCan) { bCanCloseObject = bCan; }

	void AddNumberToEnteredPin(int32 Number);
	void ChangeToUsePin(int32 PinCode);
	void PlayOpenCloseEffects();

	void PinIsCorrect(bool bClickedByOwner = false);
	void RestartDoorPanelWidget();

	void ChangeDoorStatus();

private:

	UPROPERTY(Transient)
	bool bDoorOpen = false;
	UPROPERTY(Transient)
	bool bCanCloseObject = true;

	FTimerHandle ChangeDoorStatusHandle;

	class IUsePinToEnterInterface* ActorWithWidget = nullptr;

	UPROPERTY(Transient)
	int32 CurrentPinCode = 0;
	UPROPERTY(Transient)
	FString CurrentlyEnteredPin = FString("----");
	UPROPERTY(Transient)
	FString CurrentlyEnteredPin_Text = FString("----");

};
