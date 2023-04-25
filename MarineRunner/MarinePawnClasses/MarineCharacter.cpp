// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

#include "MarineRunner/MarinePawnClasses/CroachAndSlide.h"
#include "MarineRunner/MarinePawnClasses/DashComponent.h"
#include "MarineRunner/MarinePawnClasses/TakeAndDrop.h"
#include "MarineRunner/MarinePawnClasses/Hook.h"
#include "MarineRunner/MarinePawnClasses/SwingLine.h"
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"
#include "MarineRunner/MarinePawnClasses/SlowMotionComponent.h"
#include "MarineRunner/MarinePawnClasses/PullUpComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Widgets/DashWidget.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/GunClasses/Gun.h"

// Sets default values
AMarineCharacter::AMarineCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsulePawn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CapsulePawn"));
	RootComponent = CapsulePawn;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CapsulePawn);

	CroachAndSlideComponent = CreateDefaultSubobject<UCroachAndSlide>(TEXT("CroachAndSlideComponent"));
	CroachAndSlideComponent->SetupAttachment(CapsulePawn);

	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));
	DashComponent->SetupAttachment(CapsulePawn);

	TakeAndDropComponent = CreateDefaultSubobject<UTakeAndDrop>(TEXT("TakeAndDropComponent"));
	TakeAndDropComponent->SetupAttachment(CapsulePawn);

	WallrunComponent = CreateDefaultSubobject<UWallrunComponent>(TEXT("WallrunComponent"));
	SlowMotionComponent = CreateDefaultSubobject<USlowMotionComponent>(TEXT("SlowMotionComponent"));
	PullUpComponent = CreateDefaultSubobject<UPullUpComponent>(TEXT("PullUpComponent"));
	WeaponInventoryComponent = CreateDefaultSubobject<UWeaponInventoryComponent>(TEXT("WeaponInventoryComponent"));

	Tags.Add(TEXT("Player"));

}

// Called when the game starts or when spawned
void AMarineCharacter::BeginPlay()
{
	Super::BeginPlay();

	MakeCrosshire();
	MakeHudWidget();
}

// Called every frame
void AMarineCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SwingInterp(DeltaTime);
	SwingLineCheck();

	Movement(DeltaTime);
	JumpTick(DeltaTime);
	CheckIfIsInAir();
}

// Called to bind functionality to input
void AMarineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Gun
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AMarineCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AMarineCharacter::ReleasedShoot);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMarineCharacter::Reload);

	//Aiming
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &AMarineCharacter::AimPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AMarineCharacter::AimReleased);

	//Weapon Inventory
	PlayerInputComponent->BindAction(TEXT("First_Weapon"), IE_Pressed, this, &AMarineCharacter::FirstWeapon);
	PlayerInputComponent->BindAction(TEXT("Second_Weapon"), IE_Pressed, this, &AMarineCharacter::SecondWeapon);

	//TakeAndDrop
	PlayerInputComponent->BindAction(TEXT("Take"), IE_Pressed, this, &AMarineCharacter::Take);
	PlayerInputComponent->BindAction(TEXT("Drop"), IE_Pressed, this, &AMarineCharacter::DropItem);

	//Movement
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMarineCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Croach"), IE_Pressed, this, &AMarineCharacter::CroachPressed);
	PlayerInputComponent->BindAction(TEXT("Croach"), IE_Released, this, &AMarineCharacter::CroachReleased);
	PlayerInputComponent->BindAction(TEXT("Forward"), IE_Released, this, &AMarineCharacter::ForwardReleased);

	//Gameplay components
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &AMarineCharacter::Dash);
	PlayerInputComponent->BindAction(TEXT("Swing"), IE_Pressed, this, &AMarineCharacter::SwingPressed);
	PlayerInputComponent->BindAction(TEXT("SlowMotion"), IE_Pressed, this, &AMarineCharacter::SlowMotionPressed);
	
}

void AMarineCharacter::AimPressed()
{
	if (Gun == nullptr) return;

	if (CrosshairWidget) CrosshairWidget->RemoveFromParent();

	Gun->SetCanAimTheGun(1);
}

void AMarineCharacter::AimReleased()
{
	if (Gun == nullptr) return;

	MakeCrosshire();

	Gun->SetCanAimTheGun(2);
}

void AMarineCharacter::Shoot()
{
	if (Gun == nullptr) return;

	Gun->Shoot();
	if (Gun->GetIsAutomatic()) Gun->ShouldConstantlyShoot(true);
}

void AMarineCharacter::ReleasedShoot()
{
	if (Gun == nullptr) return;

	Gun->ShootReleased();
}

void AMarineCharacter::Reload()
{
	if (Gun == nullptr) return;

	Gun->Reload();
}

void AMarineCharacter::Movement(float Delta)
{
	if (bCanSwingLerp) return;

	FVector ForwardDirection;
	FVector RightDirection;

	GoConstanlyForward(ForwardDirection, RightDirection);

	//When Pawn is close to the wall then dont add impulse in direction of this wall (thanks to that pawn will not stick to the wall)
	UnstickFromWall(ForwardDirection, RightDirection);

	float DirectionX = ForwardDirection.X - RightDirection.X;
	float DirectionY = ForwardDirection.Y - RightDirection.Y;
	FVector MovementDirection = FVector(DirectionX, DirectionY, 0);

	DisableCounterMovement(MovementDirection);

	//CounterMovement, thanks for that the pawn will not move like skating because of Physics.
	FVector Velocity = GetVelocity();
	Velocity.X *= -1.f;
	Velocity.Y *= -1.f;
	float DeltaCounterMovementForce = CounterMovementForce * Delta;
	FVector CounterMovement = FVector(DeltaCounterMovementForce * Velocity.X, DeltaCounterMovementForce * Velocity.Y, 0);

	//Add Movement
	float DeltaForce = Force * Delta * (1000 * MovementSpeedMultiplier);
	MovementDirection.Normalize();
	CapsulePawn->AddImpulse((MovementDirection * DeltaForce) + CounterMovement);
}

//Thanks to that Pawn will "Fly" after Impulse and not stopping because of CounterMovement
void AMarineCharacter::DisableCounterMovement(FVector& MovementDir)
{
	if (bShouldAddCounterMovement) return;
	
	//Check if there is obstacle in direction of MovementImpulse. If there is then The Pawn will stop "flying" and will add CounterMovement
	FHitResult HitsInCounterMovement;
	TArray<AActor*> ActorsToIgnore;
	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation() + MovementImpulse * 100.f, FVector(20, 20, 120), FRotator(0, 0, 0), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitsInCounterMovement, true))
	{
		bShouldAddCounterMovement = true;
		return;
	}

	if (GetInputAxisValue(TEXT("Forward")) < -0.1f)
	{
		MovementImpulse = FRotator(0, 100, 0).RotateVector(MovementImpulse);
	}
	MovementDir += MovementImpulse;
}

void AMarineCharacter::GoConstanlyForward(FVector& ForwardDir, FVector& RightDir)
{
	if (WallrunComponent->GetShouldPlayerGoForward())
	{
		ForwardDir = GetActorForwardVector();
		RightDir = FVector(0, 0, 0);
	}
	else
	{
		ForwardDir = GetInputAxisValue(TEXT("Forward")) * GetActorForwardVector();
		RightDir = GetInputAxisValue(TEXT("Right")) * GetActorRightVector();
	}
}

void AMarineCharacter::ForwardReleased()
{
	CroachAndSlideComponent->SetIsPressedForward(false);
	CroachAndSlideComponent->SetShouldSlide(false);
	if (GetActorScale3D().Z == 1.5f)
	{
		Force = CroachAndSlideComponent->GetCroachWalkingSpeed();
	}
}

void AMarineCharacter::Jump()
{
	if (WallrunComponent->GetCanJump() == false) return;

	if (bIsInAir == false || WallrunComponent->ShouldAddImpulseAfterWallrun(true))
	{
		bIsJumping = true;
		JumpTimeElapsed = 0;
	}
	else
	{
		bCanDelayJump = true;
		GetWorldTimerManager().SetTimer(DelayJumpHandle, this, &AMarineCharacter::DelayJump, DelayJumpTime, false);
	}
}

void AMarineCharacter::DelayJump()
{
	bCanDelayJump = false;
}

void AMarineCharacter::JumpTick(float DeltaTime)
{
	if (bIsJumping == false) return;
	
	if (JumpTimeElapsed < JumpUpTime)
	{
		FVector Start = GetActorLocation();
		Start.Z += 100.f * GetActorScale3D().Z;
		FHitResult HitR;
		//Checking if Pawn hit something overhead, if he is then Jumping is over and DownForce is applied
		if (!MakeCheckBox(FVector(4.4, 4.4, 8.5), Start, Start, HitR, false)) 
		{
			//Jumps Up
			float NewVelZ = FMath::Lerp(InitialJumpForce, -50.f, JumpTimeElapsed / JumpUpTime);

			FVector LinearVelocity = CapsulePawn->GetPhysicsLinearVelocity();
			LinearVelocity.Z = NewVelZ;

			CapsulePawn->SetPhysicsLinearVelocity(LinearVelocity);
		}
		else JumpTimeElapsed += JumpUpTime;

		WallrunComponent->AddImpulseAfterWallrun(JumpTimeElapsed);

		JumpTimeElapsed += DeltaTime;	
	}
	else if (bDownForce == false)
	{
		//Down Physics applied when TimeJump is over
		FVector DownJumpImpulse = -GetActorUpVector() * JumpDownForce * 10;
		CapsulePawn->AddImpulse(DownJumpImpulse);

		bDownForce = true;
	}
	else
	{
		bDownForce = false;
		bIsJumping = false;
	}
}

void AMarineCharacter::UnstickFromWall(FVector& ForwardDir, FVector& RightDir)
{
	if (bIsInAir == false) return;

	FHitResult HitResult;
	FVector Start = GetActorLocation();
	Start.Z += 100.f;
	FVector Size = FVector(20, 20, 120);
	TArray<AActor*> ActorsToIgnore;

	//Check in front of the pawn and behind it if there is any obstacle. If there is, then don't add a impulse in the direction the player is pressing (forward button...).
	if (GetInputAxisValue("Forward") != 0)
	{
		if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, Start + GetActorForwardVector() * 50.f, Size, FRotator(0, 0, 0), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true) //in Front of The Pawn
			|| (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, Start + -GetActorForwardVector() * 50.f, Size, FRotator(0, 0, 0), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))) //Behind
		{
			ForwardDir = FVector(0, 0, 0);
		}
	}

	//Its the same but with right direction (right button, left button)
	if (GetInputAxisValue("Right") != 0)
	{
		if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, Start + GetActorRightVector() * 50.f, Size, FRotator(0, 0, 0), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true) //Right Side
			|| (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, Start + -GetActorRightVector() * 50.f, Size, FRotator(0, 0, 0), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))) //Left Side 
		{
			RightDir = FVector(0, 0, 0);
		}
	}
}

void AMarineCharacter::CheckIfIsInAir()
{
	FHitResult Hit;
	if (!MakeCheckBox(FVector(25.f, 25.f, 2.f), GetActorLocation(), GetActorLocation(), Hit))
	{
		bIsInAir = true;
		bSlideOnRamp = false;
	}
	else
	{
		if (bIsInAir)
		{
			WallrunComponent->CallResetWallrunningAfterLanding();

			IsOnGround = true;
			bShouldAddCounterMovement = true;

			bIsInAir = false;
			PullUpComponent->SetPulledHimselfUp(false);
		}

		if (Hit.GetActor()->ActorHasTag("Ramp"))
		{
			bSlideOnRamp = true;
			//Check if Pawn is going UP on ramp, if he is then he cant slide
			if (!Hit.GetActor()->GetActorForwardVector().Equals(GetActorForwardVector(), 1.1f))
			{
				bIsGoingUp = true;
			}
			else
			{
				bIsGoingUp = false;
			}
		}
		else bSlideOnRamp = false;

		if (bCanDelayJump == true)
		{
			Jump();
			bCanDelayJump = false;
			GetWorldTimerManager().ClearTimer(DelayJumpHandle);
		}
		
	}
}

//If the player press the Swing button then spawn the Line that is going to the Hook and then wait for SwingDelay to elapsed
void AMarineCharacter::SwingPressed()
{
	if (!bCanMarineSwing || HookCast == nullptr || bCanSwingLerp == true) return;

	FVector SpawnLocation = Camera->GetComponentLocation();
	SpawnLocation += Camera->GetForwardVector() * 100.f;
	SpawnLocation += Camera->GetRightVector() * 70.f;
	ASwingLine* SwingLine = GetWorld()->SpawnActor<ASwingLine>(SwingLineClass, SpawnLocation, FRotator(0, 0, 0));
	if (SwingLine)
	{
		SwingLine->SetHookLocation(HookCast->GetActorLocation());
		SwingLine->SetSpeedLine(SwingDelay);
		SwingLine->SetCanTick(true);
	}

	GetWorldTimerManager().SetTimer(SwingHandle, this, &AMarineCharacter::Swing, SwingDelay);
}

void AMarineCharacter::Swing()
{
	if (HookCast == nullptr) return;

	CapsulePawn->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	bCanSwingLerp = true;

	HookLocation = HookCast->GetActorLocation();
	HookLocation.Z -= 50.f;

	FVector DirectionOfVector = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HookLocation));
	SwingImpulse = DirectionOfVector * SwingForce * 1000.f;
	CapsulePawn->AddImpulse(SwingImpulse);

	//Get Direction To Hook but only in X and Z Axis. Its for Pawn Physics
	SwingImpulse = FRotator(0, SwingImpulse.Rotation().Yaw, 0).Vector();
	MovementImpulse = SwingImpulse;
	bShouldAddCounterMovement = false;

	//Things that cannot happen while Swing
	MovementStuffThatCannotHappen();
}

//Interp player to location of the Hook
void AMarineCharacter::SwingInterp(float Delta)
{
	if (!bCanSwingLerp || HookCast == nullptr) return;
	
	FVector LocationInterp = FMath::VInterpTo(GetActorLocation(), HookLocation, Delta, SwingSpeed);
	if (Camera->GetComponentLocation().Equals(HookLocation, 200))
	{
		bCanSwingLerp = false;
		FVector Velocity = CapsulePawn->GetPhysicsLinearVelocity() * SwingLinearPhysicsMultiplier;
		Velocity.Z = CapsulePawn->GetPhysicsLinearVelocity().Z;
		CapsulePawn->SetPhysicsLinearVelocity(Velocity);
	}
	SetActorLocation(LocationInterp);

}

void AMarineCharacter::SwingLineCheck()
{
	if (bCanSwingLerp) return;

	FVector LineStart = Camera->GetComponentLocation();
	FVector LineEnd = LineStart + (Camera->GetForwardVector() * 2000.f);
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResults;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineStart, LineEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		AHook* TempHook;
		if (HitResults.GetActor()->ActorHasTag("Hook"))
		{
			TempHook = Cast<AHook>(HitResults.GetActor());

			FRotator FoundRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), TempHook->GetActorLocation());
			if (FoundRotation.Equals(GetControlRotation(), 60.f))
			{
				if (HookCast && TempHook != HookCast) HookCast->HookInactiv();
				TempHook->HookActivate();
				HookCast = TempHook;
				bCanMarineSwing = true;
			}
			else
			{
				bCanMarineSwing = false;
				TempHook->HookInactiv();
				HookCast = TempHook;
			}
		}
		else if (HookCast)
		{
			HookCast->HookInactiv();
			bCanMarineSwing = false;
		}
	}
	else if (HookCast)
	{
		HookCast->HookInactiv();
		bCanMarineSwing = false;
	}
}

void AMarineCharacter::CroachPressed()
{
	if (bCanSwingLerp || WallrunComponent->GetIsWallrunning() || SlowMotionComponent->GetIsInSlowMotion()) return;

	CroachAndSlideComponent->CroachPressed();
}

void AMarineCharacter::CroachReleased()
{
	CroachAndSlideComponent->CroachReleased();
}

void AMarineCharacter::FirstWeapon()
{
	if (!bCanChangeWeapon) return;

	Gun = WeaponInventoryComponent->GetWeaponFromStorage(1, Gun);
}

void AMarineCharacter::SecondWeapon()
{
	if (!bCanChangeWeapon) return;

	Gun = WeaponInventoryComponent->GetWeaponFromStorage(2, Gun);
}

void AMarineCharacter::HideGunAndAddTheNewOne(AGun* NewGun)
{
	if (Gun)
	{
		Gun->SetActorHiddenInGame(true);
		Gun->SetGunSwayWhileMovingTimer(true);
	}
	WeaponInventoryComponent->AddNewWeaponToStorage(NewGun);
}

void AMarineCharacter::Take()
{
	TakeAndDropComponent->Take();
}

void AMarineCharacter::DropItem()
{
	TakeAndDropComponent->DropItem();
}

void AMarineCharacter::Dash()
{
	if (bCanSwingLerp || WallrunComponent->GetIsWallrunning() || SlowMotionComponent->GetIsInSlowMotion()) return;

	bShouldAddCounterMovement = true;
	DashComponent->Dash();
}

void AMarineCharacter::SlowMotionPressed()
{
	if (!bIsInAir || WallrunComponent->GetIsWallrunning() || bCanSwingLerp) return;

	SlowMotionComponent->TurnOnSlowMotion();
}

bool AMarineCharacter::MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult &OutHitResult, bool bDebug)
{
	if (bDebug) DrawDebugBox(GetWorld(), NewStart, FVector(Size), FColor::Red, true);
	return GetWorld()->SweepSingleByChannel(OutHitResult, NewStart, NewEnd, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(Size));
}

bool AMarineCharacter::MakeCheckLine(FHitResult &OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug, FColor Color)
{
	if (bDebug) DrawDebugLine(GetWorld(), NewStart, NewEnd, Color, false, 0.5f);
	return GetWorld()->LineTraceSingleByChannel(OutHitResult, NewStart, NewEnd, ECC_Visibility);
}

FVector AMarineCharacter::EaseInQuint(FVector Start, FVector End, float Alpha)
{
	Alpha--;
	End -= Start;
	return End * (Alpha * Alpha * Alpha * Alpha * Alpha + 1) + Start;
}

void AMarineCharacter::MakeHudWidget()
{
	APlayerController* MarineController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (HUDClass && MarineController)
	{
		HudWidget = Cast<UHUDWidget>(CreateWidget(MarineController, HUDClass));
		HudWidget->AddToViewport();
	}
}

void AMarineCharacter::MakeCrosshire()
{
	APlayerController* MarineController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (CrosshairClass && MarineController)
	{
		CrosshairWidget = CreateWidget(MarineController, CrosshairClass);
		CrosshairWidget->AddToViewport();
	}
}

void AMarineCharacter::MovementStuffThatCannotHappen()
{
	bIsJumping = false;
	CroachReleased();
}

void AMarineCharacter::GotDamage(float Damage)
{
	if (!HudWidget) return;

	Health -= Damage;
	if (Health < 0.f) Health = 0.f;
	HudWidget->SetHealthPercent();

	HudWidget->SetGotDamage(true);
}

void AMarineCharacter::MakeDashWidget(bool bShouldMake, float FadeTime, bool bAddFov)
{
	APlayerController* MarineController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (DashClass)
	{
		if (bShouldMake)
		{
			DashWidget = Cast<UDashWidget>(CreateWidget(MarineController, DashClass));
			DashWidget->SetFadeTime(FadeTime);
			DashWidget->ShouldAddChangingFov(bAddFov);
			DashWidget->AddToViewport();
		}	
	}
}
