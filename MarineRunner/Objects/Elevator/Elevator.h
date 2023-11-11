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

	void StartElevator(FVector Location);

	//  DELETE
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetShouldMove() const { return bShouldMove; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ElevatorMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* ElevatorDoorsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* ElevatorPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float StartElevatorDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float CanUseElevatorAgainDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Settings")
		float TimeToMoveOnFloor = 4.f;
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

	FVector StartLocation;
	FVector FloorLocationToGo;
	bool bShouldMove;
	float MoveTimeElapsed;
	void MoveToFloor(float Delta);

	void MovedToNewFoor();

	void PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay);

};
