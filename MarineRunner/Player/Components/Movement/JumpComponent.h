// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "JumpComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UJumpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UJumpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Jump();
	void TurnOffJump(bool bDelayJumpToo = false);

	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }
	FORCEINLINE bool GetIsGoingUp() const { return bIsGoingUpOnRamp; }
	FORCEINLINE bool GetIsOnRamp() const { return bIsOnRamp; }
	FORCEINLINE bool GetWasOnRamp() const { return bWasOnRamp; }
	FORCEINLINE bool GetIsJumping() const { return bStartApplyingJumpForces; }
	FORCEINLINE float GetDividerForMovementWhenInAir() const { return DividerForMovementWhenInAir; }
	FORCEINLINE float GetDividerForCounterForceWhenInAir() const { return DividerForCounterForceWhenInAir; }

private:
	//InitialJumpForce is moving to EndJumpForce and then applied to Velocity Z
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	float InitialJumpForce = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	float EndJumpForce = -50.f;
	//Impuls force that is applied to Player in down vector, Thanks to this player is falling faster
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	float JumpDownForce = 150000.f;
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	float JumpUpTime = 0.35f;
	//Time for Delay Jump
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	float DelayJumpTime = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	float DelayIsInAirTime = 0.4f;
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
	TObjectPtr<USoundBase> JumpSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
	float CheckInAirTime = 0.001f;
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
	FVector SomethingIsBelowBoxSize = FVector(25.f, 25.f, 30.f);
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
	FVector SomethingIsBelowBoxSizeWhenInCrouch = FVector(25.f, 25.f, 60.f);
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
	FVector SomethingIsUpBoxSize = FVector(25.f, 25.f, 25.f);
	// Divide Movement speed and CounterMovementForce by this value when in Air
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
	float DividerForCounterForceWhenInAir = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
	float DividerForMovementWhenInAir = 8.f;

	// The curve that will be applied (set) to the camera relative location when the player hits the ground for the first time.
	UPROPERTY(EditDefaultsOnly, Category = "On Ground Settings")
	TObjectPtr<UCurveFloat> ImpactOnFloorCameraEffectCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "On Ground Settings")
	TObjectPtr<USoundBase> ImpactOnFloorSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "On Ground Settings")
	float UpdatePreviousLocationOnRampTime = 0.05f;
	UFUNCTION()
	void ImpactOnFloorCameraEffectTimelineProgress(float CurveValue);

	UPROPERTY(Transient)
	bool bStartApplyingJumpForces = false;
	UPROPERTY(Transient)
	bool bDownForceWasApplied = false;
	UPROPERTY(Transient)
	float JumpTimeElapsed = 0.f;
	bool CanJump();
	void ApplyJumpForces(const float& DeltaTime);
	void ApplyJumpUPForce(const float& DeltaTime);

	UPROPERTY(Transient)
	bool bCanDelayJump = false;
	FTimerHandle DelayJumpHandle;
	void DelayJump();
	FORCEINLINE void DisableCanDelayJump() { bCanDelayJump = false; };

	UPROPERTY(Transient)
	bool bIsInAir = false;
	FTimerHandle CheckInAirHandle;
	void FirstMomentInAir();
	void CheckIfIsInAir();
	void FirstTimeOnGround();

	void PlayerOnRamp(const FHitResult& GroundHitResult);
	void CheckIfPlayerIsGoingUpOnRamp();
	void DisablePlayerOnRampActions();
	UPROPERTY(Transient)
	bool bIsOnRamp = false;
	// when player jumps off ramp then bWasOnRamp == true, after landing on stable ground then bWasOnRamp == false
	UPROPERTY(Transient)
	bool bWasOnRamp = false;
	UPROPERTY(Transient)
	bool bIsGoingUpOnRamp = false;
	UPROPERTY(Transient)
	FVector PreviousPlayerLocationOnRamp = FVector::Zero();
	UPROPERTY(Transient)
	FTimerHandle PreviousPlayerLocationHandle;
	void UpdatePreviousPlayerLocationOnRamp();

	// Impact on Floor
	void LandingEffect();
	void CreateImpactOnFloorTimeline();
	FTimeline ImpactOnFloorTimeline;

	UPROPERTY(Transient)
	bool bDelayIsInAir = false;
	FTimerHandle DelayIsInAirHandle;
	FORCEINLINE void DisableDelayIsInAir() { bDelayIsInAir = false; };

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
};
