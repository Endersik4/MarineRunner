// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"



UCLASS()
class MARINERUNNER_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartElevator(FVector Location, int32 Floor);

	//  DELETE
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetShouldMove() const { return bShouldMove; }

	FORCEINLINE class UElevatorPanelWidget* GetElevatorPanelWidget() const { return ElevatorPanelWidget; }
	FORCEINLINE int32 GetCurrentFloor() const { return FloorToGo; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ElevatorMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* ElevatorDoorsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* ElevatorPanel;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		float StartElevatorDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float CanUseElevatorAgainDelay;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		float TimeToMoveOnFloor = 4.f;
	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
		TMap<int32, class AOutsideElevatorDoor*> OutsideElevatorDoors;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
		UAnimationAsset* OpenElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Animations")
		UAnimationAsset* CloseElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		FName SoundLocationSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings|Sounds")
		USoundBase* OpenElevatorDoorsSound;

	class UElevatorPanelWidget* ElevatorPanelWidget;

	// Start Elevator Delay
	FTimerHandle StartElevatorHandle;
	void StartMovingElevator();

	void CanUseElevatorAgain();

	int32 FloorToGo;
	FVector StartLocation;
	FVector FloorLocationToGo;
	bool bShouldMove;
	float MoveTimeElapsed;
	void MoveToFloor(float Delta);

	class AOutsideElevatorDoor* CurrentOutsideElevatorDoor;

	bool bDoorOpen = false;

	void MovedToNewFoor();

	void PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay);

};
