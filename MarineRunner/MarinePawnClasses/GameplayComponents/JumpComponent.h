// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JumpComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	FORCEINLINE bool GetIsGoingUp() const { return bIsGoingUp; }
	FORCEINLINE bool GetIsOnRamp() const { return bIsOnRamp; }
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }
	FORCEINLINE float GetDividerForMovementWhenInAir() const { return DividerForMovementWhenInAir; }
	FORCEINLINE float GetDividerForCounterForceWhenInAir() const { return DividerForCounterForceWhenInAir; }

private:
	//InitialJumpForce is lerping to -50.f and then applied to Velocity Z
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
		float InitialJumpForce = 1800.f;
	//Impuls force that is applied to Player in down vector, Thanks to this player is falling faster
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
		float JumpDownForce = 15000.f;
	//How Fast (in seconds) lerp will be done 
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
		float JumpUpTime = 0.35f;
	//Time for Delay Jump
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
		float DelayJumpTime = 0.15f;
	UPROPERTY(EditAnywhere, Category = "Jump Settings")
		float DelayIsInAirTime = 0.3f;
	UPROPERTY(EditAnywhere, Category = "Jump Settings|Sounds")
		USoundBase* JumpSound;

	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
		FVector BoxSizeToCheckIfSomethingIsBelow = FVector(25.f, 25.f, 2.f);
	// Divide Movement speed and CounterMovementForce by this value when in Air
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
		float DividerForCounterForceWhenInAir = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "In Air Settings")
		float DividerForMovementWhenInAir = 8.f;

	// The curve that will be applied (set) to the camera relative location when the player hits the ground for the first time.
	UPROPERTY(EditDefaultsOnly, Category = "On Ground Settings")
		UCurveFloat* ImpactOnFloorCameraEffectCurve;
	UPROPERTY(EditDefaultsOnly, Category = "On Ground Settings|Sounds")
		USoundBase* ImpactOnFloorSound;
	UFUNCTION()
		void ImpactOnFloorCameraEffectTimelineProgress(float CurveValue);

	//Jumps Variables
	bool bDownForce;
	bool bIsJumping;
	float JumpTimeElapsed;
	void JumpTick(float DeltaTime);

	//Delayed jump
	bool bCanDelayJump;
	FTimerHandle DelayJumpHandle;
	void DelayJump();
	void SetCanDelayJump() { bCanDelayJump = false; };

	//In Air
	bool bIsInAir;
	void CheckIfIsInAir();
	void FirstTimeOnGround();
	void PlayerOnRamp(const FHitResult& GroundHitResult);

	void LandingEffect();
	void CreateImpactOnFloorTimeline();
	FTimeline ImpactOnFloorTimeline;

	//Delay is in Air (can jump though player is not in the air)
	bool bDelayIsInAir;
	FTimerHandle DelayIsInAirHandle;
	void SetDelayIsInAir() { bDelayIsInAir = false; }

	bool bIsOnRamp;
	bool bIsGoingUp;
		
	AMarineCharacter* Player;
};
