// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Objects/Interactive/PanelWithPin/UsePinToEnterInterface.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "Door.generated.h"

class UDoorPanelWidget;
UCLASS()
class MARINERUNNER_API ADoor : public AActor, public IUsePinToEnterInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ClickedOpenButton(class UDoorPanelWidget* ClickedWidget = nullptr) override;
	virtual void PinCorrect() override;
	virtual class UDoorPanelWidget* GetOtherPanelWidget(class UDoorPanelWidget* PanelActivatedByPlayer) override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class USkeletalMeshComponent> DoorSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UWidgetComponent> DoorPanelWidgetComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> DoorPanelMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UWidgetComponent> DoorPanelSecondWidgetComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> DoorPanelSecondMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
		float CloseDoorAfterInactivityTime = 4.f;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
		bool bUsePinCode = false;
	UPROPERTY(EditAnywhere, Category = "Door Settings", meta = (EditCondition = "bUsePinCode", EditConditionHides, ClampMin = "1000", ClampMax = "9999", UIMin = "1000", UIMax = "9999"))
		int32 PinCode = 1111;

	UPROPERTY(EditDefaultsOnly, Category = "Door Settings")
		FName DoorSoundSocketName = FName();
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Open")
		TObjectPtr < UAnimationAsset> OpenDoorAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Open")
		TObjectPtr < USoundBase> OpenDoorSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Closed")
		TObjectPtr < UAnimationAsset> CloseDoorAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Closed")
		TObjectPtr < USoundBase> CloseDoorSound = nullptr;

	UPROPERTY(Transient)
		bool bDoorOpen = false;

	FTimerHandle CloseAfterInactivityHandle;
	void CloseDoorsAfterInactivity();

	void OpenDoor();
	void CloseDoor();

	UPROPERTY(Transient)
		TObjectPtr < UDoorPanelWidget> DoorPanelWidget = nullptr;
	UPROPERTY(Transient)
		TObjectPtr < UDoorPanelWidget> DoorPanelSecondWidget = nullptr;
	void SetUpDoorPanels();

	UPROPERTY(Transient)
		int32 CurrentUniqueID = 0;
	void StopUsingPin();
};
