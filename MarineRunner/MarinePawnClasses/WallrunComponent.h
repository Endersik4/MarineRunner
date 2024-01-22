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

	UFUNCTION(BlueprintCallable)
		void SetShouldLerpRotation(bool bShould) { bShouldLerpRotation = bShould; }

	UFUNCTION(BlueprintCallable)
		bool GetIsWallrunning() const { return bIsWallrunning; }

	//Getters
	FORCEINLINE bool GetCanJump() const { return bCanJumpWhileWallrunning; }
	FORCEINLINE FVector GetWallrunDirection() const { return WallrunDirection; }

	bool ShouldAddImpulseAfterWallrun(bool bShould); //Check If Should Add This Impulse. Return true if Pawn is wallrunning, false otherwise
	void AddImpulseAfterWallrun(float JumpTimeElapsed); //When Player jumps while in wallrunning then Add Impulse to push player away from Obstacle.
	//JumpTimeElapsed is the elapsed time when the player pressed the Jump button

	void CallResetWallrunningAfterLanding();//When Player is trying to do a wallrun but very close to the floor then Wallrun is disabled

private:
	//Speed of Pawn while performing wallrun
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float WallrunSpeed = 1.5f;
	//Impulse added to Stick with Obstacle
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float StickWithObstacleImpulse = 2300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float JumpFromWallrunImpulse = 780000.f;
	//How much should change the angle of the impact vector
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float AngleOfHitImpact = 85.f;

	//Interp Speed of Changing the Yaw Camera
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Wallrun Begins Camera Rotation")
		float CameraYawSpeed = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Wallrun Begins Camera Rotation")
		UCurveFloat* CameraRollRightSideCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun|Wallrun Begins Camera Rotation")
		UCurveFloat* CameraRollLeftSideCurve;
	UFUNCTION()
		void CameraRollTimelineProgress(float CurveValue);

	//Wallrunning
	bool bIsWallrunning;
	bool bShouldAddImpulseAfterWallrun;
	bool bCanJumpWhileWallrunning = true;
	FVector WallrunDirection;

	float WallrunTimeElapsed = 0.6f; //After the jump, this time must pass to do the wallrun again
	FVector WallrunningWhereToJump; //ImpactNormal of Obstacle HitResult
	void SetCanJumpWhileWallrunning();

	//Yaw camera rotation after run-up begins
	bool bShouldLerpRotation;
	float WhereToInterp;
	void RotateCameraYaw(ESideOfLine CurrentSide, FVector HitNormal);
	void CameraRotationInterp(float Delta);

	// Rotate camera roll in direction of the obstacle normal 
	ESideOfLine CurrentRotatedCameraRoll;
	void RotateCameraWhileWallrunning(UCurveFloat* CurveToUse);
	FTimeline RotateCameraRollTimeline;

	bool bCameraRollWasRotated = false;

	//Wallrunning Functions
	void Wallrunning(float Delta);
	void ResetWallrunning(); //Disable Wallrun
	void StickToTheObstacle(ESideOfLine CurrentSide, FVector HitNormal);
	bool IsPawnNextToObstacle(FVector& HitNormal, ESideOfLine& OutCurrentSide, ESideOfLine WhichSideToLook = ESideOfLine::Left);
	bool CanDoWallrun(float Delta);

	FRotator PlayerRotationWallrun;
	FRotator PlayerRotationWhileWallrun;

	//Other
	FTimerHandle CanJumpHandle;

	class AMarineCharacter* MarinePawn;
	APlayerController* PlayerController;
};