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

	FORCEINLINE class UElevatorPanelWidget* GetElevatorPanelWidget() const { return ElevatorPanelWidget; }
	FORCEINLINE int32 GetCurrentFloor() const { return CurrentFloor; }
	FORCEINLINE bool GetDoorOpen() const {return bDoorOpen;}


private: 
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ElevatorMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* ElevatorDoorsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* ElevatorPanel;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		TArray<FElevatorFloor> ElevatorFloors;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		int32 CurrentFloor = 1;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		float TimeToCloseDoorsAfterInactivity = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float CanUseElevatorAgainDelay = 4.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		float TimeToMoveOnFloor = 4.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		TMap<int32, class AOutsideElevatorDoor*> OutsideElevatorDoors;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
		UAnimationAsset* OpenElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
		UAnimationAsset* CloseElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		FName DoorSoundSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		USoundBase* OpenElevatorDoorsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		USoundBase* AmbientElevatorSound;

	UPROPERTY(EditAnywhere, Category = "Cutscenes")
		bool bGoToTheFloorAfterTime = false;
	UPROPERTY(EditAnywhere, Category = "Cutscenes")
		float MaxDistanceToPlayerToDoCutscene = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Cutscenes", meta = (EditCondition = "bGoToTheFloorAfterTime", EditConditionHides))
		float MoveToFloorTimeInCutscene = 1.f;
	UPROPERTY(EditAnywhere, Category = "Cutscenes", meta = (EditCondition = "bGoToTheFloorAfterTime", EditConditionHides))
		FElevatorFloor FloorToMoveWhileCutscene;

	class UElevatorPanelWidget* ElevatorPanelWidget;

	void CanUseElevatorAgain();

	// Start Elevator Delay
	FTimerHandle StartElevatorHandle;
	void StartMovingElevator();

	// Moving Elevator
	bool bCanMoveToFloorLocation;
	float MoveTimeElapsed;
	FVector StartLocation;
	FVector FloorLocationToGo;
	void ElevatorIsMoving(float Delta);
	void MovedToNewFoor();

	bool bDoorOpen = false;

	void PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay);
	void CloseElevatorDoors();
	void ActivateElevatorDoors();

	void SetUpElevatorPanel();

	FTimerHandle CloseDoorAfterInactivityHandle;

	// Cutscene
	void MoveToFloorAfterTime();
	bool IsPlayerTooFarAwayToDoCutscene();

	class AOutsideElevatorDoor* CurrentOutsideElevatorDoor;
	class AOutsideElevatorDoor* BeforeCurrentOutsideElevatorDoor;
	void OpenOutsideElevatorDoors();

	// Save/Load elevator
	void SaveElevatorState(int32 SaveState);
	int32 CurrentUniqueID = 0;

	class UAudioComponent* SpawnedAmbientElevatorSound;
};
