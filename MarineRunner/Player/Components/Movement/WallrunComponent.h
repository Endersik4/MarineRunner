// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"

#include "WallrunComponent.generated.h"

UENUM()
enum ESideOfLine {
	Left,
	Right
};

/// <summary>
/// A Component that allow MarineCharacter to do Wallrun on any object wall
/// In Details Panel you can set up variables for Wallrun (speed,impulse,angle,camera rotate speed)
/// </summary>
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UWallrunComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWallrunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetShouldCameraYawRotate(bool bShould) { bRotateYawCameraTowardsWallrun = bShould;}
	FORCEINLINE void SetCanJumpAfterWallrun(bool bCan) { bCanJumpAfterWallrun = bCan; }

	//Getters
	FORCEINLINE bool GetCanJump() const { return bCanJumpWhileWallrunning; }
	FORCEINLINE bool GetCanJumpAfterWallrun() const { return bCanJumpAfterWallrun; }
	FORCEINLINE FVector GetWallrunDirection() const { return WallrunDirection; }
	FORCEINLINE bool GetIsWallrunning() const { return bIsWallrunning; }

	void AddImpulseAfterWallrun(); //When Player jumps while in wallrunning then Add Impulse to push player away from Obstacle.

	void CallResetWallrunningAfterLanding();//When Player is trying to do a wallrun but very close to the floor then Wallrun is disabled

private:
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
	FFloatRange WallrunVelocityRange = FFloatRange(2000.f, 2500.f);
	//Force added to Stick with Obstacle
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float StickWithObstacleImpulse = 120000.f;
	//How much should change the angle of the impact vector
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float AngleOfHitImpact = 85.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float DelayToStartNextWallrun = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float MaxYawDistanceToStopWallrunning = 45.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Jump")
		float JumpFromWallrunImpulse = 780000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Jump")
		float CanJumpWhenWallrunStartsTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Jump")
		float CanJumpAfterWallrunTime = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Conditions to perform wallrun")
		float ObstacleRaycastDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Conditions to perform wallrun")
		FFloatRange VelocityRangeToStopWallrunming = FFloatRange(-5.f, 400.f);
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Conditions to perform wallrun")
		float MinVelocityToPerformWallrun = 120.f;

	//Interp Speed of Changing the Yaw Camera
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Wallrun Begins Camera Rotation")
		float CameraYawSpeed = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Wallrun Begins Camera Rotation")
		TObjectPtr<UCurveFloat> CameraRollRightSideCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Wallrun Begins Camera Rotation")
		TObjectPtr<UCurveFloat> CameraRollLeftSideCurve = nullptr;
	UFUNCTION()
		void CameraRollTimelineProgress(float CurveValue);

	UPROPERTY(Transient)
	bool bIsWallrunning = false;
	UPROPERTY(Transient)
	bool bCanJumpWhileWallrunning = true;
	UPROPERTY(Transient)
	FVector WallrunDirection = FVector::Zero();

	UPROPERTY(Transient)
		float WallrunVelocity = 0.f;
	UPROPERTY(Transient)
		float WallrunTimeElapsed = 0.6f; //After the jump, this time must pass to do the wallrun again
	UPROPERTY(Transient)
		FVector WallrunningWhereToJump = FVector::Zero(); //ImpactNormal of Obstacle HitResult
	void SetCanJumpWhileWallrunning();
	FTimerHandle CanJumpHandle;

	//Yaw camera rotation after run-up begins
	UPROPERTY(Transient)
		bool bRotateYawCameraTowardsWallrun = false;
	UPROPERTY(Transient)
		float RotateYawCameraAngle = 0.f;
	void RotateCameraYaw(ESideOfLine CurrentSide, FVector HitNormal);
	void CameraRotationTowardsHitNormal(float Delta);

	// Rotate camera roll in direction of the obstacle normal 
	UPROPERTY(Transient)
		bool bCameraRollWasRotated = false;
	ESideOfLine CurrentRotatedCameraRoll;
	void RotateCameraWhileWallrunning(UCurveFloat* CurveToUse);
	FTimeline RotateCameraRollTimeline;

	//Wallrunning Functions
	void Wallrunning(float Delta);
	void ResetWallrunning(); //Disable Wallrun
	void StickToTheObstacle(ESideOfLine CurrentSide, FVector HitNormal);
	void BeginWallrun(ESideOfLine CurrentSide, FVector HitNorma);
	bool IsPawnNextToObstacle(FVector& HitNormal, ESideOfLine& OutCurrentSide, ESideOfLine WhichSideToLook = ESideOfLine::Left);
	bool CanDoWallrun(float Delta);

	// coyote time after wallrun
	UPROPERTY(Transient)
		bool bCanJumpAfterWallrun = false;
	FTimerHandle CanJumpAfterWallrunHandle;
	void DisableCanJumpAfterWallrun();

	UPROPERTY(Transient)
		FRotator PlayerRotationWallrun = FRotator::ZeroRotator;
	UPROPERTY(Transient)
		FRotator PlayerRotationWhileWallrun = FRotator::ZeroRotator;

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<APlayerController> PlayerController = nullptr;
};