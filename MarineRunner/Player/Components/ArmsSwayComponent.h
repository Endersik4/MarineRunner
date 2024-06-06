// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArmsSwayComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UArmsSwayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UArmsSwayComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetArmsSwayDivider(float NewSwayDivider) { ArmsSwayDivider = NewSwayDivider; }
	FORCEINLINE void SetArmsSwayWhileMovingDivider(float NewSwayWhileMovingDivder) { ArmsSwayWhileMovingDivider = NewSwayWhileMovingDivder; }

	void ResetArmsLocation();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float BackToInitialPositionSpeed = 1.f;
	//Max Pitch rotation of arms when player looking down
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float RotationSwayPitchRangeBack = -2.f;
	//Max Pitch rotation of arms when player looking up
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float RotationSwayPitchRangeUp = 2.f;
	//Arms to Sway pitch rotation speed 
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float SpeedOfSwayPitch = 7.f;

	//Max Yaw rotation of Arms when player looking left
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float RotationSwayYawRangeBack = -2.75f;
	//Maximal Yaw rotation of Arms when player looking right
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float RotationSwayYawRangeUp = 2.75f;
	//Arms to Sway yaw rotation speed 
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway")
	float SpeedOfSwayYaw = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float MinVelocityToStartArmsSway = 150.f;
	//Max X location of Arms when player moving forward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float LocationSwayXRangeBack = -3.f;
	//Max X location of Arms when player moving backward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float LocationSwayXRangeUp = 3.f;
	//Arms to Sway X location speed 
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float SpeedOfSwayX = 7.f;

	//Max Y location of Arms when player moving right (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float LocationSwayYRangeBack = -2.f;
	//Max Y location of Arms when player moving left (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float LocationSwayYRangeUp = 2.f;
	//Arms to Sway Y location speed
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float SpeedOfSwayY = 4.f;

	//Speed of Moving Arms while Player is moving
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float SpeedOfSwayWhileMoving = 550.f;
	//Multiplier of Calculeted Y by Lemniscate Of Bernoulli, this represents how big Arms is gonna move in Y
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float MultiplierOfLocationYSwayWhileMoving = 0.8f;
	//Multiplier of Calculeted Z by Lemniscate Of Bernoulli, this represents how big Arms is gonna move in Z
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float MultiplierOfLocationZSwayWhileMoving = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Arms Sway While Moving")
	float SwayWhileMovingDividerInCutscene = 4.f;

	float ArmsSwayDivider = 1.f;
	float ArmsSwayWhileMovingDivider = 1.f;

	UPROPERTY(Transient)
	FRotator ArmsRotationSway = FRotator::ZeroRotator;
	UPROPERTY(Transient)
	FVector InitialArmsRelativeLocation = FVector::Zero();
	void CalculateArmsSway(FVector& CalculatedLocation, FRotator& CalculatedRotation, const float& Delta);
	void ArmsSway_MouseMove(float Delta);

	FVector CalculateArmsSwayWhileMoving();
	void ArmsSway_WhileMoving();

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
};
