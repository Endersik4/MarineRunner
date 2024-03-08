// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelData.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "Elevator.generated.h"

UCLASS()
class MARINERUNNER_API AElevator : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PrepareElevatorToMove(FVector Location, int32 Floor);

	FORCEINLINE TObjectPtr<class UElevatorPanelWidget> GetElevatorPanelWidget() const { return ElevatorPanelWidget; }
	FORCEINLINE int32 GetCurrentFloor() const { return CurrentFloor; }
	FORCEINLINE bool GetDoorOpen() const {return bDoorOpen;}

private: 
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> ElevatorMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class USkeletalMeshComponent> ElevatorDoorsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UWidgetComponent> ElevatorPanel;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		TArray<FElevatorFloor> ElevatorFloors;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		int32 CurrentFloor = 1;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		float TimeToMoveOnFloor = 4.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		float TimeToCloseDoorsAfterInactivity = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float CanUseElevatorAgainDelay = 4.f;
	// int32 - floor numer of outside elevator
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		TMap<int32, TObjectPtr<class AOutsideElevatorDoor>> OutsideElevatorDoors;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
		TObjectPtr < UAnimationAsset> OpenElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
		TObjectPtr<UAnimationAsset> CloseElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		FName DoorSoundSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		TObjectPtr<USoundBase> OpenElevatorDoorsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		TObjectPtr<USoundBase> AmbientElevatorSound;

	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play")
		bool bMoveElevatorAfterTime = false;
	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play")
		float MaxDistanceToPlayerToDoCutscene = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play", meta = (EditCondition = "bMoveElevatorAfterTime", EditConditionHides))
		float MoveToFloorTimeInCutscene = 1.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play", meta = (EditCondition = "bMoveElevatorAfterTime", EditConditionHides))
		FElevatorFloor FloorToMoveWhileCutscene;

	UPROPERTY(Transient)
		TObjectPtr<class UElevatorPanelWidget> ElevatorPanelWidget;

	void CanUseElevatorAgain();

	// Start Elevator Delay
	FTimerHandle StartElevatorHandle;
	void StartMovingElevator();

	// Moving Elevator
	bool bCanMoveToFloorLocation;
	float MoveElevatorTimeElapsed;
	FVector StartLocation;
	FVector FloorLocationToGo;
	void ElevatorIsMoving(float Delta);
	void MovedToNewFloor();

	bool bDoorOpen = false;

	void PlayElevatorEffects(TObjectPtr < UAnimationAsset> AnimToPlay, TObjectPtr < USoundBase> SoundToPlay);
	void CloseElevatorDoors();
	void ActivateElevatorDoors();

	void SetUpElevatorPanel();

	FTimerHandle CloseDoorAfterInactivityHandle;

	// Cutscene
	void MoveElevatorAfterTime();
	bool IsPlayerTooFarAwayToDoCutscene();
	void OpenOutsideElevatorDoors();

	UPROPERTY(Transient)
		TObjectPtr<class AOutsideElevatorDoor> CurrentOutsideElevatorDoor;
	UPROPERTY(Transient)
		TObjectPtr<class AOutsideElevatorDoor> BeforeCurrentOutsideElevatorDoor;

	// Save/Load elevator
	void SaveElevatorState(int32 SaveState);
	int32 CurrentUniqueID = 0;

	UPROPERTY(Transient)
		TObjectPtr<class UAudioComponent> SpawnedAmbientElevatorSound;
};	
