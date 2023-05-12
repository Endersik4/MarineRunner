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
#include "MarineRunner/Objects/Checkpoint.h"
#include "MarineRunner/Framework/SaveMarineRunner.h"

// Sets default values
AMarineCharacter::AMarineCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsulePawn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CapsulePawn"));
	RootComponent = CapsulePawn;

	CapsulePawn->SetSimulatePhysics(true);
	CapsulePawn->SetMassScale(NAME_None, 2.f);
	CapsulePawn->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	CapsulePawn->SetCastShadow(false);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CapsulePawn);
	Camera->bUsePawnControlRotation = true;

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

	bUseControllerRotationYaw = true;
	Tags.Add(TEXT("Player"));

}

// Called when the game starts or when spawned
void AMarineCharacter::BeginPlay()
{
	Super::BeginPlay();

	MakeCrosshire();
	MakeHudWidget();
	CopyOfOriginalForce = MovementForce;
}

// Called every frame
void AMarineCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SwingInterp();
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
	PlayerInputComponent->BindAction(TEXT("Action"), IE_Pressed, this, &AMarineCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Action"), IE_Released, this, &AMarineCharacter::ReleasedShoot);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMarineCharacter::Reload);

	PlayerInputComponent->BindAction(TEXT("FirstAidKit"), IE_Pressed, this, &AMarineCharacter::UseFirstAidKit);

	//Aiming
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Pressed, this, &AMarineCharacter::ADSPressed);
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Released, this, &AMarineCharacter::ADSReleased);

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

	//Gameplay components
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &AMarineCharacter::Dash);
	PlayerInputComponent->BindAction(TEXT("Swing"), IE_Pressed, this, &AMarineCharacter::SwingPressed);
	PlayerInputComponent->BindAction(TEXT("SlowMotion"), IE_Pressed, this, &AMarineCharacter::SlowMotionPressed);
	
}

void AMarineCharacter::UseFirstAidKit()
{
	if (FirstAidKits <= 0 || bCanUseFirstAidKit == false || Health == 100.f) return;

	FirstAidKits--;
	Health += FirstAidKitHealth;
	if (Health > 100.f) Health = 100.f;

	if (UseFirstAidKitSound) UGameplayStatics::SpawnSound2D(GetWorld(), UseFirstAidKitSound);

	HudWidget->SetHealthPercent();
	HudWidget->SetCurrentNumberOfFirstAidKits();
	HudWidget->SetDidPlayerUseFirstAidKit(true);

	ElementBar ProgressHealBar{ DelayAfterUseFirstAidKit }, ProgressHealButtonBar{ 0.3f };
	HudWidget->AddElementToProgress(EUseableElement::Heal, ProgressHealBar);
	HudWidget->AddElementToProgress(EUseableElement::Button_Heal, ProgressHealButtonBar);

	bCanUseFirstAidKit = false;
	GetWorldTimerManager().SetTimer(FirstAidKitHandle, this, &AMarineCharacter::CanUseFirstAidKit, DelayAfterUseFirstAidKit, false);
}

void AMarineCharacter::ADSPressed()
{
	if (Gun == nullptr || WallrunComponent->GetIsWallrunning()) return;

	if (CrosshairWidget)
	{
		CrosshairWidget->RemoveFromParent();
		CrosshairWidget = nullptr;
	}

	if (ADSInSound) UGameplayStatics::SpawnSound2D(GetWorld(), ADSInSound);
	bIsPlayerADS = true;
	Gun->SetStatusOfGun(StatusOfAimedGun::ADS);
}

void AMarineCharacter::ADSReleased()
{
	if (Gun == nullptr || WallrunComponent->GetIsWallrunning()) return;

	MakeCrosshire();

	if (ADSOutSound) UGameplayStatics::SpawnSound2D(GetWorld(), ADSOutSound);
	bIsPlayerADS = false;
	Gun->SetStatusOfGun(StatusOfAimedGun::BackToInitialPosition);
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

	Gun->WaitToReload();
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
	if (bSlideOnRamp && CroachAndSlideComponent->GetIsSliding()) MovementDirection += 1.f * -GetActorUpVector();

	DisableCounterMovement(MovementDirection);

	//CounterMovement, thanks for that the pawn will not move like skating because of Physics.
	FVector Velocity = GetVelocity();
	Velocity.X *= -1.f;
	Velocity.Y *= -1.f;
	float DeltaCounterMovementForce = CounterMovementForce * Delta;
	FVector CounterMovement = FVector(DeltaCounterMovementForce * Velocity.X, DeltaCounterMovementForce * Velocity.Y, 0);

	//Add Movement
	float DeltaForce = (bIsPlayerADS ? (MovementForce / DividerOfMovementWhenADS) : MovementForce) * Delta * (1000 * MovementSpeedMultiplier);
	MovementDirection.Normalize();
	CapsulePawn->AddImpulse((MovementDirection * DeltaForce) + CounterMovement);
	
	PlayFootstepsSound();
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

void AMarineCharacter::Jump()
{
	if (WallrunComponent->GetCanJump() == false) return;

	if (bIsInAir == false || WallrunComponent->ShouldAddImpulseAfterWallrun(true))
	{
		bIsJumping = true;
		JumpTimeElapsed = 0;
		if (JumpSound) UGameplayStatics::SpawnSound2D(GetWorld(), JumpSound);
	}
	else
	{
		bCanDelayJump = true;
		GetWorldTimerManager().SetTimer(DelayJumpHandle, this, &AMarineCharacter::SetCanDelayJump, DelayJumpTime, false);
	}
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
	//Check if there is ground under the player, if not, the player is in the air
	if (!MakeCheckBox(FVector(25.f, 25.f, 2.f), GetActorLocation(), GetActorLocation(), Hit))
	{
		bIsInAir = true;
		bSlideOnRamp = false;
	}
	else
	{
		//The first moment a player touches the ground
		if (bIsInAir)
		{
			WallrunComponent->CallResetWallrunningAfterLanding();

			IsOnGround = true;
			bShouldAddCounterMovement = true;

			bIsInAir = false;
			PullUpComponent->SetPulledHimselfUp(false);

			if (ImpactOnFloorSound) UGameplayStatics::SpawnSoundAttached(ImpactOnFloorSound, CapsulePawn);
			
			MovementForce = CopyOfOriginalForce;
			MovementSpeedMultiplier = 1.f;
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

		DelayJump();
		
	}
}

void AMarineCharacter::PlayFootstepsSound()
{
	if (bCanPlayFootstepsSound == false) return;
	if (WallrunComponent->GetIsWallrunning() == false && bIsInAir == true) return;
	if (CroachAndSlideComponent->GetIsSliding() == true) return;

	if ((GetInputAxisValue(TEXT("Forward")) != 0.f || GetInputAxisValue(TEXT("Right")) != 0.f) || WallrunComponent->GetIsWallrunning() == true)
	{
		float TimeOfHandle = 0.21f;
		if (WallrunComponent->GetIsWallrunning() == true && FootstepsWallrunSound)
		{
			TimeOfHandle = 0.17f;
			UGameplayStatics::SpawnSound2D(GetWorld(), FootstepsWallrunSound);
		}
		else if (bIsCroaching == true && FootstepsCroachSound)
		{
			TimeOfHandle = 0.43f;
			UGameplayStatics::SpawnSoundAttached(FootstepsCroachSound, CapsulePawn);
		}
		else if (FootstepsSound) UGameplayStatics::SpawnSoundAttached(FootstepsSound, CapsulePawn);

		bCanPlayFootstepsSound = false;
		GetWorldTimerManager().SetTimer(FootstepsHandle, this, &AMarineCharacter::SetCanPlayFootstepsSound, TimeOfHandle, false);
	}
}

//If the player press the Swing button then spawn the Line that is going to the Hook and then wait for SwingDelay to elapsed
void AMarineCharacter::SwingPressed()
{
	if (!bCanMarineSwing || HookCast == nullptr || bCanSwingLerp == true || bIsSwingPressed == true) return;

	bIsSwingPressed = true;
	bShouldCheckForSwing = false;
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

	if (SwingSound) UGameplayStatics::SpawnSound2D(GetWorld(), SwingSound);

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
void AMarineCharacter::SwingInterp()
{
	if (!bCanSwingLerp || HookCast == nullptr) return;
	
	FVector LocationInterp = FMath::VInterpTo(GetActorLocation(), HookLocation, GetWorld()->GetDeltaSeconds(), SwingSpeed);
	if (Camera->GetComponentLocation().Equals(HookLocation, 200))
	{
		bIsSwingPressed = false;
		bCanSwingLerp = false;
		bShouldCheckForSwing = true;
		FVector Velocity = CapsulePawn->GetPhysicsLinearVelocity() * SwingLinearPhysicsMultiplier;
		Velocity.Z = CapsulePawn->GetPhysicsLinearVelocity().Z;
		CapsulePawn->SetPhysicsLinearVelocity(Velocity);
	}
	SetActorLocation(LocationInterp);

}

void AMarineCharacter::SwingLineCheck()
{
	if (bCanSwingLerp || bShouldCheckForSwing == false) return;

	FVector LineStart = Camera->GetComponentLocation();
	FVector LineEnd = LineStart + (Camera->GetForwardVector() * 3500.f);
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

	bIsCroaching = true;
	CroachAndSlideComponent->CroachPressed();
}

void AMarineCharacter::CroachReleased()
{
	bIsCroaching = false;
	CroachAndSlideComponent->CroachReleased();
}

void AMarineCharacter::FirstWeapon()
{
	if (!bCanChangeWeapon) return;
	if (WeaponInventoryComponent->GetWeaponFromStorage(1, Gun) == Gun) return;

	Gun = WeaponInventoryComponent->GetWeaponFromStorage(1, Gun);
	if (QuickSelectSound) UGameplayStatics::SpawnSound2D(GetWorld(), QuickSelectSound);
}

void AMarineCharacter::SecondWeapon()
{
	if (!bCanChangeWeapon) return;
	if (WeaponInventoryComponent->GetWeaponFromStorage(2, Gun) == Gun) return;

	Gun = WeaponInventoryComponent->GetWeaponFromStorage(2, Gun);
	if (QuickSelectSound) UGameplayStatics::SpawnSound2D(GetWorld(), QuickSelectSound);
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
	if (bCanSwingLerp || WallrunComponent->GetIsWallrunning() || SlowMotionComponent->GetIsInSlowMotion() || bIsCroaching) return;

	bShouldAddCounterMovement = true;
	DashComponent->Dash();
}

void AMarineCharacter::SlowMotionPressed()
{
	if (!bIsInAir || WallrunComponent->GetIsWallrunning() || bCanSwingLerp) return;

	SlowMotionComponent->TurnOnSlowMotion();
}

void AMarineCharacter::LoadCheckpoint()
{
	if (CurrentCheckpoint == nullptr) return;

	LoadGame();
	SetActorLocation(CurrentCheckpoint->GetActorLocation());
}

void AMarineCharacter::SaveGame()
{
	USaveMarineRunner* SaveGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));
	TArray<AGun*> GeneratedGuns;
	WeaponInventoryComponent->ReturnAllWeapons().GenerateValueArray(GeneratedGuns);
	SaveGameInstance->SaveGame(FirstAidKits, Health, GeneratedGuns);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);
}

void AMarineCharacter::LoadGame()
{
	USaveMarineRunner* SaveGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));
	SaveGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::LoadGameFromSlot(TEXT("MySlot"), 0));
	SaveGameInstance->LoadGame(this);
	HudWidget->SetHealthPercent();
	HudWidget->SetCurrentNumberOfFirstAidKits();
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
	if (CrosshairWidget) return;

	APlayerController* MarineController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (CrosshairClass && MarineController)
	{
		CrosshairWidget = CreateWidget(MarineController, CrosshairClass);
		CrosshairWidget->AddToViewport();
	}
}

void AMarineCharacter::MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings)
{
	bIsJumping = false;
	CroachReleased();

	if (!bShouldCancelGameplayThings) return;

	if (bCanDelayJump) DelayJump();
	if (bIsPlayerADS) ADSReleased();
}

void AMarineCharacter::GotDamage(float Damage)
{
	if (!HudWidget) return;

	Health -= Damage;
	if (Health < 0.f)
	{
		Health = 0.f;
		LoadCheckpoint();
	}
	HudWidget->SetHealthPercent();

	HudWidget->SetGotDamage(true);
}

void AMarineCharacter::DelayJump()
{
	if (bCanDelayJump == false) return;

	Jump();
	bCanDelayJump = false;
	GetWorldTimerManager().ClearTimer(DelayJumpHandle);
}

void AMarineCharacter::MakeDashWidget(bool bShouldMake, float FadeTime, bool bAddFov, bool bAddChromaticAbberation)
{
	APlayerController* MarineController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (DashClass)
	{
		if (bShouldMake)
		{
			DashWidget = Cast<UDashWidget>(CreateWidget(MarineController, DashClass));
			DashWidget->SetFadeTime(FadeTime);
			DashWidget->ShouldAddChangingFov(bAddFov);
			DashWidget->ShouldAddChromaticAbberation(bAddChromaticAbberation);
			DashWidget->AddToViewport();
		}	
	}
}

bool AMarineCharacter::GetIsWallrunning() const
{
	return WallrunComponent->GetIsWallrunning();
}
