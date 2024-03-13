// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "OutsideElevatorDoor.generated.h"

enum ECallElevatorAction
{
	ECEA_ShowCall,
	ECEA_HideCallAndShowWait,
	ECEA_HideCall,
	ECEA_ShowWaitEffect,
	ECEA_HideWaitEffect,
};

UCLASS()
class MARINERUNNER_API AOutsideElevatorDoor : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOutsideElevatorDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:	

	void OpenOutsideElevatorDoor();
	void CloseOutsideElevatorDoor();

	bool CanCallElevator() const;
	void CallElevatorAction(ECallElevatorAction ActionToDo);

	void PinIsCorrect();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class USkeletalMeshComponent> OutsideElevatorDoorsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> OutsideElevatorPanelMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UWidgetComponent> OutsideElevatorPanel;

	UPROPERTY(EditAnywhere, Category = "Outside Elevator Doors settings")
		TObjectPtr<class AElevator> ElevatorToCall;
	UPROPERTY(EditAnywhere, Category = "Outside Elevator Doors settings")
		int32 FloorNumber;
	UPROPERTY(EditAnywhere, Category = "Outside Elevator Doors settings")
		bool bUsePinCode = false;
	UPROPERTY(EditAnywhere, Category = "Outside Elevator Doors settings", meta = (EditCondition = "bUsePinCode", EditConditionHides, ClampMin = "1000", ClampMax = "9999", UIMin = "1000", UIMax = "9999"))
		int32 PinCode = 1111;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Animations")
		TObjectPtr < UAnimationAsset> OpenElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Animations")
		TObjectPtr < UAnimationAsset> CloseElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Sounds")
		FName SoundLocationSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Sounds")
		TObjectPtr<USoundBase> OpenElevatorDoorsSound;

	UPROPERTY(Transient)
		TObjectPtr<class UCallElevatorPanel> ElevatorPanelWidget;

	bool bDoorsOpen = false;
	void PlayElevatorEffects(TObjectPtr < UAnimationAsset> AnimToPlay, TObjectPtr < USoundBase> SoundToPlay);

	void SetUpElevatorPanel();

	int32 CurrentUniqueID = 0;
};
