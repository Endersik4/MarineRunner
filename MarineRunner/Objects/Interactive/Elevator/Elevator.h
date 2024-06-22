// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Objects/Interactive/Elevator/ElevatorPanel/ElevatorFloorData.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

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
	FORCEINLINE bool GetDoorOpen() const { return bDoorOpen; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> ElevatorMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> ElevatorDoorsSkeletalMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> ElevatorPanel = nullptr;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
	TArray<FElevatorFloor> ElevatorFloors = { FElevatorFloor() };
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
	TMap<int32, TObjectPtr<class AOutsideElevatorDoor>> OutsideElevatorDoors = { {0, nullptr} };
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
	TObjectPtr < UAnimationAsset> OpenElevatorDoorsAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
	TObjectPtr<UAnimationAsset> CloseElevatorDoorsAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
	FName DoorSoundSocketName = FName(TEXT("DoorSoundLocation"));
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
	TObjectPtr<USoundBase> OpenElevatorDoorsSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
	TObjectPtr<USoundBase> AmbientElevatorSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play")
	bool bMoveElevatorAfterTime = false;
	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play")
	float MaxDistanceToPlayerToDoCutscene = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play", meta = (EditCondition = "bMoveElevatorAfterTime", EditConditionHides))
	float MoveToFloorTimeInCutscene = 1.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Moves at the begin play", meta = (EditCondition = "bMoveElevatorAfterTime", EditConditionHides))
	FElevatorFloor FloorToMoveWhileCutscene = FElevatorFloor();

	UPROPERTY(Transient)
	TObjectPtr<class UElevatorPanelWidget> ElevatorPanelWidget = nullptr;

	void CanUseElevatorAgain();

	// Start Elevator Delay
	FTimerHandle StartElevatorHandle;
	void StartMovingElevator();

	UPROPERTY(Transient)
	FVector FloorLocationToGo = FVector::Zero();
	void MovedToNewFloor();
	UPROPERTY(Transient)
	FTimerHandle NewFloorHandle = FTimerHandle();

	UPROPERTY(Transient)
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
	TObjectPtr<class AOutsideElevatorDoor> CurrentOutsideElevatorDoor = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AOutsideElevatorDoor> BeforeCurrentOutsideElevatorDoor = nullptr;

	// Save/Load elevator
	void SaveElevatorState(int32 SaveState);
	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;

	UPROPERTY(Transient)
	TObjectPtr<class UAudioComponent> SpawnedAmbientElevatorSound = nullptr;
};
