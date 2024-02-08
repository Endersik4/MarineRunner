// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Objects/Door/UsePinToEnterInterface.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

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

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* DoorSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* DoorPanelWidgetComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* DoorPanelMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* DoorPanelSecondWidgetComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* DoorPanelSecondMesh;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
		TSubclassOf<class ASavedDataObject> SavedDataObjectClass;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
		float CloseDoorAfterInactivityTime = 4.f;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
		bool bUsePinCode = false;
	UPROPERTY(EditAnywhere, Category = "Chest Settings", meta = (EditCondition = "bUsePinCode", EditConditionHides, ClampMin = "1000", ClampMax = "9999", UIMin = "1000", UIMax = "9999"))
		int32 PinCode = 1111;

	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Animations")
		UAnimationAsset* OpenDoorAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Animations")
		UAnimationAsset* CloseDoorAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Sounds")
		FName DoorSoundSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Sounds")
		USoundBase* OpenDoorSound;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Sounds")
		USoundBase* CloseDoorSound;

	bool bDoorOpen = false;

	FTimerHandle CloseAfterInactivityHandle;
	void CloseDoorsAfterInactivity();

	void OpenDoor();
	void CloseDoor();

	UDoorPanelWidget* DoorPanelWidget;
	UDoorPanelWidget* DoorPanelSecondWidget;
	void SetUpDoorPanels();

	int32 CurrentUniqueID = 0;
	void StopUsingPin();
};
