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

	FORCEINLINE void SetDoorActor(class ADoor* NewDoorActor) { DoorActor = NewDoorActor; }
private:

	bool bDoorOpen = false;
	class ADoor* DoorActor;
};
