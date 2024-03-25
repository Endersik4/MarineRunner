// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API USwingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool GetIsPlayerMovingToHookPosition() const { return bIsPlayerMovingToHook; }
	void SwingPressed();
private:
	//Impuls before Interp
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingImpulse = 1200000.f;
	//Multiplier that is multiplying X and Y Velocity after Pawn got to Hook
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingLinearPhysicsMultiplier = 1.5f;
	//Pawn Interp Speed From Current location to Hook Location
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingDelay = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float MaxHookDistanceToFinishMoving = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float LengthOfSwingLineRaycast = 4500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		TSubclassOf<class ASwingLine> SwingLineClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float ForwardSwingLineLocationOffset = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float RightSwingLineLocationOffset = 40.f;

	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		TObjectPtr<USoundBase> SwingSound = nullptr;

	UPROPERTY(Transient)
		bool bWasSwingPressed = false;
	UPROPERTY(Transient)
		bool bCanPlayerSwing = false;
	UPROPERTY(Transient)
		bool bCanMakeSwingLineCheck = true;

	FTimerHandle StartSwingHandle;

	void HookLineCheck();
	void ActivateCurrentHoveredHook(TObjectPtr<AActor> HookActorFromHit);
	void ClearLastActivatedHook();

	UPROPERTY(Transient)
		bool bIsPlayerMovingToHook = false;
	UPROPERTY(Transient)
		FVector HookLocation;
	void PrepareMoveToHook();
	void MovingToHook(float Delta);
	void StopMovingToHook();

	void SpawnSwingEffects();

	UPROPERTY(Transient)
		TObjectPtr<class AHook>CurrentFocusedHook = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<APlayerController> PlayerController = nullptr;
};
