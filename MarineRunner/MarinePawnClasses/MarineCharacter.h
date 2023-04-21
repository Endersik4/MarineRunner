// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineCharacter.generated.h"

UCLASS()
class MARINERUNNER_API AMarineCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMarineCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Shoot();

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class UStaticMeshComponent* CapsulePawn;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
		class UCameraComponent* Camera;

	UFUNCTION(BlueprintPure)
		class UWallrunComponent* GetWallrunComponent() const { return WallrunComponent; }

	UFUNCTION(BlueprintImplementableEvent)
		void RotateCameraWhileWallrunning(bool bIsRightSide = false);

	//Is there Marine on The ground after begin in Air, It is used in Timeline in Blueprints
	UPROPERTY(VisibleAnywhere, Category = "Movement|Jump", BlueprintReadWrite)
		bool IsOnGround;

	float GetForce() const { return Force; }
	bool GetIsJumping() const { return bIsJumping; }
	bool GetIsOnRamp() const { return bSlideOnRamp; }
	bool GetIsGoingUp() const { return bIsGoingUp; }
	bool GetIsInAir() const { return bIsInAir; }
	bool GetShouldAddCounterMovement() const { return bShouldAddCounterMovement; }
	class UCameraComponent* GetCamera() const { return Camera; }

	void SetForce(float NewForce) { Force = NewForce; }
	void SetHasWeapon(bool bNewHasWeapon) { bHasWeapon = bNewHasWeapon; }
	void SetGun(class AGun* NewGun) { Gun = NewGun; }

	void SetMovementImpulse(FVector NewImpulse) { MovementImpulse = NewImpulse; }
	void SetShouldAddCounterMovement(bool bShould) { bShouldAddCounterMovement = bShould; }
	void SetMovementSpeedMutliplier(float NewSpeed) { MovementSpeedMultiplier = NewSpeed; }

	void MovementStuffThatCannotHappen();

	void MakeDashWidget(bool bShouldMake, float FadeTime, bool bAddFov = true);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCroachAndSlide* CroachAndSlideComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UDashComponent* DashComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UTakeAndDrop* TakeAndDropComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWallrunComponent* WallrunComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USlowMotionComponent* SlowMotionComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UPullUpComponent* PullUpComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Actors to Spawn")
		TSubclassOf<class ASwingLine> SwingLineClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> DashClass;


	//Aka speed movement
	UPROPERTY(EditAnywhere, Category = "Movement")
		float Force = 2000.f;
	//Prevent from Sliding on the ground like Fish
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CounterMovementForce = 2000.f;

	//InitialJumpForce is lerping to -50.f and then applied to Velocity Z
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float InitialJumpForce = 1800.f;
	//Impuls force that is applied to Player in down vector, Thanks to this player is falling faster
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float JumpDownForce = 200.f;
	//How Fast (in seconds) lerp will be done 
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float JumpUpTime = 0.35f;
	//Time for Delay Jump
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float DelayJumpTime = 3.1f;

	//Impuls before Interp/*
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingForce = 900.f;
	//Multiplier that is multiplying X and Y Velocity after Pawn got to Hook
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingLinearPhysicsMultiplier = 1.5f;
	//Pawn Interp Speed From Pawn location to Hook Locaiton
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingSpeed = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingDelay = 0.2f;

	
	
	//Movement
	float MovementSpeedMultiplier = 1.f;
	bool bShouldAddCounterMovement = true;
	FVector MovementImpulse;
	void Movement(float Delta);
	void DisableCounterMovement(FVector& MovementDir);
	void UnstickFromWall(FVector& ForwardDir, FVector& RightDir);
	void GoConstanlyForward(FVector& ForwardDir, FVector& RightDir);
	void ForwardReleased();

	//Jumps Variables
	bool bDownForce;
	bool bIsJumping;
	float JumpTimeElapsed;
	void Jump();
	void JumpTick(float DeltaTime);

	//Delayed jump
	bool bCanDelayJump;
	FTimerHandle DelayJumpHandle;
	void DelayJump();

	//In Air
	bool bIsInAir;
	void CheckIfIsInAir();

	//Dashing
	void Dash();

	//Croaching
	bool bSlideOnRamp;
	bool bIsGoingUp;
	void CroachPressed();
	void CroachReleased();

	//Taking Items
	void Take();
	void DropItem();

	//Gun
	bool bHasWeapon;
	
	void ReleasedShoot();
	void Reload();
	class AGun* Gun;

	//Aiming
	void AimPressed();
	void AimReleased();
	
	//Swing
	bool bCanMarineSwing;
	bool bCanSwingLerp;
	FVector HookLocation;
	FVector SwingImpulse;
	FTimerHandle SwingHandle;
	void SwingPressed();
	void Swing();
	void SwingInterp(float Delta);
	void SwingLineCheck();
	class AHook* HookCast;

	//SlowMotion
	void SlowMotionPressed();

	//Widgets
	void MakeCrosshire();
	UUserWidget* CrosshairWidget;
	class UDashWidget* DashWidget;

	//Checking Stuff
	bool MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult &OutHitResult, bool bDebug = false);
	bool MakeCheckLine(FHitResult &OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug = false, FColor Color = FColor::Red);

	FVector EaseInQuint(FVector Start, FVector End, float Alpha);

};
