// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorPanelWidget.generated.h"

UCLASS()
class MARINERUNNER_API UDoorPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* InteractDoorButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* InteractDoorText;

	// If pin is in use
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundPinImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* PinCodeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* PinNumbersTileView;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* OnClickedInteractDoorAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* HoverInteractDoorAnim = nullptr;

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
		float TimeToChangeDoorStatusText = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		USoundBase* WrongCodeSound;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TArray<int32> PinNumberEntries;
	UPROPERTY(EditDefaultsOnly, Category = "Door Panel Settings|Pin")
		TSubclassOf<class UPinNumberEntryObject> PinNumberEntryObjectClass;

	FORCEINLINE void SetDoorActor(class IUsePinToEnterInterface* NewActorWithWidget) { ActorWithWidget = NewActorWithWidget; }
	FORCEINLINE void SetCanCloseObject(bool bCan) { bCanCloseObject = bCan; }

	void AddNumberToEnteredPin(int32 Number);
	void ChangeDoorPanelToUsePin(int32 PinCode);
	void PlayOpenCloseEffects();

	void PinIsCorrect(bool bClickedByOwner = false);

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
