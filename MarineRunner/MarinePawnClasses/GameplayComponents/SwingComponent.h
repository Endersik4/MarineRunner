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

	//Impuls before Interp
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings")
		float SwingForce = 1200000.f;
	//Multiplier that is multiplying X and Y Velocity after Pawn got to Hook
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings")
		float SwingLinearPhysicsMultiplier = 1.5f;
	//Pawn Interp Speed From Current location to Hook Location
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings")
		float SwingSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings")
		float SwingDelay = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings")
		TSubclassOf<class ASwingLine> SwingLineClass;
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings|Hook")
		float MaxHookDistanceToFinishInterp = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings|Raycast")
		float LengthOfSwingLineRaycast = 4500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings|Effects")
		float ForwardSwingLineOffset = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing Settings|Effects")
		float RightSwingLineOffset = 40.f;

	UPROPERTY(EditDefaultsOnly, Category = "Swing Sounds")
		USoundBase* SwingSound;

	bool GetIsPlayerLerpingToHookPosition() const { return bIsPlayerLerpingToHookPosition; }
	void SwingPressed();
private:

	bool bWasSwingPressed = false;
	bool bCanPlayerSwing;
	bool bCanMakeSwingLineCheck = true;
	bool bIsPlayerLerpingToHookPosition;

	FVector HookLocation;
	FVector SwingImpulse;
	FTimerHandle SwingHandle;

	void StartSwingToHook();
	void HookLineCheck();
	void ActivateCurrentHoveredHook(AActor* HookActorFromHit);

	void SwingInterp(float Delta);
	void StopSwingInterp();

	void SpawnSwingEffects();

	void ClearLastActivatedHook();

	class AHook* CurrentFocusedHook;
	
	class AMarineCharacter* MarinePlayer;
	APlayerController* PlayerController;
};
