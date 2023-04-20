// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallrunComponent.generated.h"

/// <summary>
/// A Component that allow MarineCharacter to do Wallrun on any object wall
/// In Details Panel you can set up variables for Wallrun (speed,impulse,angle,camera rotate speed)
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	bool GetShouldPlayerGoForward() const { return bShouldPlayerGoForward; }



	bool GetCanJump() const { return bCanJumpWhileWallrunning; }

	bool ShouldAddImpulseAfterWallrun(bool bShould); //Check If Should Add This Impulse. Return true if Pawn is wallrunning, false otherwise
	void AddImpulseAfterWallrun(float JumpTimeElapsed); //When Player jumps while in wallrunning then Add Impulse to push player away from Obstacle.
	//JumpTimeElapsed is the elapsed time when the player pressed the Jump button
	
	void CallResetWallrunningAfterLanding();//When Player is trying to do a wallrun but very close to the floor then Wallrun is disabled

private:
	//Speed of Pawn while performing wallrun
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float WallrunSpeed = 1.3f;

	//Impulse added to Stick with Obstacle
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float StickWithObstacleImpulse = 2000.f;

	//How much should change the angle of the impact vector
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float AngleOfHitImpact = 85.f;
	//Interp Speed of Changing the Yaw Camera
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun")
		float CameraYawSpeed = 8.f;

	class AMarineCharacter* MarinePawn;

	//Wallrunning
	bool bIsWallrunning;
	bool bShouldAddImpulseAfterWallrun;
	bool bCanJumpWhileWallrunning = true;
	bool bShouldPlayerGoForward; //If true then in the Movement function in MarineCharacter the player will always walk forward

	float WallrunTimeElapsed = 0.6f; //After the jump, this time must pass to do the wallrun again
	FVector WallrunningWhereToJump; //ImpactNormal of Obstacle HitResult
	void SetCanJumpWhileWallrunning();

	//Yaw camera rotation after run-up begins
	bool bShouldLerpRotation;
	float WhereToInterp;
	void RotateCameraYaw(int32 WhichSide, FVector HitNormal);
	void CameraRotationInterp(float Delta);

	//Functions
	void Wallrunning(); 
	void ResetWallrunning(); //Disable Wallrun
	void StickToTheObstacle(int32 WhichSide, FVector HitNormal);
	bool IsPawnNextToObstacle(int32 &WhichSide, FVector& HitNormal); //If WhichSide == -1 then left, if == 1 then Right
	bool ShouldDoWallrun();

	//Other
	bool MakeCheckLine(FHitResult& OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug = false, FColor Color = FColor::Red);
	FTimerHandle CanJumpHandle;

		
};
