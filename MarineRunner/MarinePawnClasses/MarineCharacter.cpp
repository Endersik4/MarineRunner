// Copyright Adam Bartela.All Rights Reserved

#include "MarineCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

#include "MarineRunner/MarinePawnClasses/GameplayComponents/JumpComponent.h"
#include "MarineRunner/MarinePawnClasses/CroachAndSlide.h"
#include "MarineRunner/MarinePawnClasses/DashComponent.h"
#include "MarineRunner/MarinePawnClasses/TakeAndDrop.h"
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"
#include "MarineRunner/MarinePawnClasses/SlowMotionComponent.h"
#include "MarineRunner/MarinePawnClasses/PullUpComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/SwingComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/SpawnDeathWidgetComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/SaveLoadPlayerComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/ArmsSwayComponent.h"

// Sets default values
AMarineCharacter::AMarineCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

		CapsulePawn = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Pawn Component"));
	RootComponent = CapsulePawn;

	CapsulePawn->SetSimulatePhysics(true);
	//CapsulePawn->SetMassScale(NAME_None, 2.f);
	CapsulePawn->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	CapsulePawn->SetCollisionProfileName(FName(TEXT("PlayerCapsule")));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CapsulePawn);
	Camera->bUsePawnControlRotation = true;

	ArmsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms Skeletal Mesh"));
	ArmsSkeletalMesh->SetupAttachment(Camera);

	JumpComponent = CreateDefaultSubobject<UJumpComponent>(TEXT("JumpComponent"));
	CroachAndSlideComponent = CreateDefaultSubobject<UCroachAndSlide>(TEXT("CroachAndSlideComponent"));

	TakeAndDropComponent = CreateDefaultSubobject<UTakeAndDrop>(TEXT("TakeAndDropComponent"));

	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));
	WallrunComponent = CreateDefaultSubobject<UWallrunComponent>(TEXT("WallrunComponent"));
	SlowMotionComponent = CreateDefaultSubobject<USlowMotionComponent>(TEXT("SlowMotionComponent"));
	PullUpComponent = CreateDefaultSubobject<UPullUpComponent>(TEXT("PullUpComponent"));
	WeaponInventoryComponent = CreateDefaultSubobject<UWeaponInventoryComponent>(TEXT("WeaponInventoryComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	PauseMenuComponent = CreateDefaultSubobject<UPauseMenuComponent>(TEXT("PauseMenuComponent"));
	SwingComponent = CreateDefaultSubobject<USwingComponent>(TEXT("SwingComponent"));
	WeaponHandlerComponent = CreateDefaultSubobject<UWeaponHandlerComponent>(TEXT("WeaponHandlerComponent"));
	SpawnDeathWidgetComponent = CreateDefaultSubobject<USpawnDeathWidgetComponent>(TEXT("SpawnDeathWidgetComponent"));
	MessageHandlerComponent = CreateDefaultSubobject<UMessageHandlerComponent>(TEXT("MessageHandlerComponent"));
	SaveLoadPlayerComponent = CreateDefaultSubobject<USaveLoadPlayerComponent>(TEXT("Save and Load Player Component"));
	ArmsSwayComponent = CreateDefaultSubobject<UArmsSwayComponent>(TEXT("Arms Sway Component"));

	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComponent"));
	WidgetInteractionComponent->SetupAttachment(Camera);
	WidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::CenterScreen;

	bUseControllerRotationYaw = true;
	Tags.Add(TEXT("Player"));
}

// Called when the game starts or when spawned
void AMarineCharacter::BeginPlay()
{
	Super::BeginPlay();

	MarinePlayerController = Cast<AMarinePlayerController>(GetController());
	PauseMenuComponent->ChangeUIToGameOnly();

	MakeCrosshire();
	MakeHudWidget();

	if (IsValid(HudWidget))
	{
		HudWidget->SetHealthBarPercent(Health);
		FItemStruct* FirstAidKitItem = InventoryComponent->GetItemFromInventory(FirstAidKitRowName);
		HudWidget->SetCurrentNumberOfFirstAidKits(FirstAidKitItem ? FirstAidKitItem->Item_Amount : 0);
	}

	ReplaceRootComponentRotation();
}

// Called every frame
void AMarineCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMarineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Gun
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::Shoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Released, WeaponHandlerComponent, &UWeaponHandlerComponent::ReleasedShoot);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::Reload);

	PlayerInputComponent->BindAxis(TEXT("Zoom"), WeaponHandlerComponent, &UWeaponHandlerComponent::Zoom);

	PlayerInputComponent->BindAction(TEXT("FirstAidKit"), IE_Pressed, this, &AMarineCharacter::UseFirstAidKit);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AMarineCharacter::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AMarineCharacter::Right);

	//Aiming
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::ADSPressed);
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Released, WeaponHandlerComponent, &UWeaponHandlerComponent::ADSReleased);

	//Weapon Inventory
	PlayerInputComponent->BindAction<FSelectWeaponDelegate>(TEXT("First_Weapon"), IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::SelectWeaponFromQuickInventory, 1);
	PlayerInputComponent->BindAction<FSelectWeaponDelegate>(TEXT("Second_Weapon"), IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::SelectWeaponFromQuickInventory, 2);

	//TakeAndDrop
	PlayerInputComponent->BindAction(TEXT("Take"), IE_Pressed, this, &AMarineCharacter::KeyEPressed);
	PlayerInputComponent->BindAction(TEXT("Take"), IE_Released, this, &AMarineCharacter::KeyEReleased);
	//PlayerInputComponent->BindAction(TEXT("Drop"), IE_Pressed, TakeAndDropComponent, &UTakeAndDrop::DropItem);

	//Movement
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, JumpComponent, &UJumpComponent::Jump);
	PlayerInputComponent->BindAction<FCrouchSlideDelegate>(TEXT("Crouch"), IE_Pressed, CroachAndSlideComponent, &UCroachAndSlide::CrouchPressed, false);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, CroachAndSlideComponent, &UCroachAndSlide::CrouchReleased);

	PlayerInputComponent->BindAction<FCrouchSlideDelegate>(TEXT("Slide"), IE_Pressed, CroachAndSlideComponent, &UCroachAndSlide::CrouchPressed, true);
	PlayerInputComponent->BindAction(TEXT("Slide"), IE_Released, CroachAndSlideComponent, &UCroachAndSlide::CrouchReleased);

	//Gameplay components
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, DashComponent, &UDashComponent::Dash);
	PlayerInputComponent->BindAction(TEXT("Swing"), IE_Pressed, SwingComponent, &USwingComponent::SwingPressed);
	PlayerInputComponent->BindAction(TEXT("SlowMotion"), IE_Pressed, SlowMotionComponent, &USlowMotionComponent::TurnOnSlowMotion);

	// Menu
	FInputActionBinding& MainMenuToggle = PlayerInputComponent->BindAction(TEXT("MainMenu"), IE_Pressed, PauseMenuComponent, &UPauseMenuComponent::PauseGame);
	MainMenuToggle.bExecuteWhenPaused = true;

	FInputActionBinding& CloseMessageToggle = PlayerInputComponent->BindAction<FCloseMessageDelegate>(TEXT("CloseMessage"), IE_Pressed, MessageHandlerComponent, &UMessageHandlerComponent::DeleteCurrentDisplayedMessage, this);
	CloseMessageToggle.bExecuteWhenPaused = true;

	// You Died Bindings
	FInputActionBinding& YouDiedRestartToggle = PlayerInputComponent->BindAction(TEXT("RestartGameWhenDead"), IE_Pressed, SpawnDeathWidgetComponent, &USpawnDeathWidgetComponent::RestartGameInYouDiedWidget);
	YouDiedRestartToggle.bExecuteWhenPaused = true;
	FInputActionBinding& YouDiedQuitToggle = PlayerInputComponent->BindAction(TEXT("QuitGameWhenDead"), IE_Pressed, SpawnDeathWidgetComponent, &USpawnDeathWidgetComponent::QuitGameInYouDiedWidget);
	YouDiedQuitToggle.bExecuteWhenPaused = true;

	PlayerInputComponent->BindAction(TEXT("CallAlbertos"), IE_Pressed, this, &AMarineCharacter::CallAlbertosPressed);
}

void AMarineCharacter::ChangeMouseSensitivity(const FSettingSavedInJsonFile& NewMouseSensitivity, bool bResetMouseSensitivity)
{
	if (IsValid(MarinePlayerController) == false)
		return;

	if (bResetMouseSensitivity == true)
	{
		MarinePlayerController->SetMouseSensitivity(MouseSensitivityJSON);
		return;
	}

	MarinePlayerController->SetMouseSensitivity(NewMouseSensitivity);
}

#pragma region //////////////////////////////// MOVEMENT ///////////////////////////////
void AMarineCharacter::Forward(float Axis)
{
	// There is a bug where ForwardVector is not correct and had to use Controller->RootComponent but this way have other bug, when player looks up or down then he cant 
	// go forward/backward and to fix this i had to rotate RightVector by -90.f
	FVector Dir = MarinePlayerController->GetRootComponent()->GetRightVector().RotateAngleAxis(DegreeForForwardVector, FVector(0.f, 0.f, 1.f));

	if (bShouldPlayerGoForward == true)
	{
		Axis = 1.f;
	}
	if (GetIsWallrunning() == true)
	{
		Dir = WallrunComponent->GetWallrunDirection();
	}

	Move(Dir, Axis, FName(TEXT("Right")));
}
void AMarineCharacter::Right(float Axis)
{
	Move(MarinePlayerController->GetRootComponent()->GetRightVector(), Axis, FName(TEXT("Forward")));
}
void AMarineCharacter::Move(FVector Direction, float Axis, const FName InputAxisName)
{
	float Speed = MovementForce / (GetInputAxisValue(InputAxisName) != 0.f ? 1.3f : 1);

	if (GetIsWallrunning() == true)
	{
		Speed = MovementForce * MovementSpeedMutliplier;
	}
	//if (bIsOnRamp && CroachAndSlideComponent->GetIsSliding()) MovementDirection += 1.f * -GetActorUpVector();
	if (GetIsInAir() == true && GetIsWallrunning() == false)
	{
		Speed /= JumpComponent->GetDividerForMovementWhenInAir();
	}

	Direction.Z = 0.f;

	FVector Force = (Axis * Direction * Speed) + CalculateCounterMovement();

	PlayFootstepsSound();
	//float DeltaForce = (WeaponHandlerComponent->GetIsPlayerInAds() ? (MovementForce / DividerOfMovementWhenADS) : MovementForce) * Delta * (1000 * MovementSpeedMultiplier);

	CapsulePawn->AddImpulse(Force);
}
FVector AMarineCharacter::CalculateCounterMovement()
{
	FVector Velocity = GetVelocity();
	Velocity.X *= -1.f;
	Velocity.Y *= -1.f;
	float CounterForce = CounterMovementForce;
	if (GetIsInAir() == true && GetIsWallrunning() == false)
	{
		CounterForce = CounterMovementForce / JumpComponent->GetDividerForCounterForceWhenInAir();
	}

	return FVector(CounterForce * Velocity.X, CounterForce * Velocity.Y, 0);
}
#pragma endregion

#pragma region //////////////////////////// FOOTSTEPS SOUND ////////////////////////////
void AMarineCharacter::PlayFootstepsSound()
{
	if (bCanPlayFootstepsSound == false || (GetVelocity().Length() >= 0.f && GetVelocity().Length() <= 150.f))
		return;
	if (WallrunComponent->GetIsWallrunning() == false && GetIsInAir() == true)
		return;
	if (CroachAndSlideComponent->GetIsSliding() == true)
		return;

	if ((GetInputAxisValue(TEXT("Forward")) != 0.f || GetInputAxisValue(TEXT("Right")) != 0.f) || WallrunComponent->GetIsWallrunning() == true)
	{
		float TimeOfHandle = 0.21f;
		if (WallrunComponent->GetIsWallrunning() == true && FootstepsWallrunSound)
		{
			TimeOfHandle = 0.17f;
			UGameplayStatics::SpawnSound2D(GetWorld(), FootstepsWallrunSound);
		}
		else if (GetIsCrouching() == true && FootstepsCroachSound)
		{
			TimeOfHandle = 0.43f;
			UGameplayStatics::SpawnSoundAttached(FootstepsCroachSound, CapsulePawn);
		}
		else if (FootstepsSound) UGameplayStatics::SpawnSoundAttached(FootstepsSound, CapsulePawn);

		bCanPlayFootstepsSound = false;
		GetWorldTimerManager().SetTimer(FootstepsHandle, this, &AMarineCharacter::SetCanPlayFootstepsSound, TimeOfHandle, false);
	}
}

#pragma endregion 

#pragma region ///////////////////////////// FIRST AID KIT /////////////////////////////

void AMarineCharacter::UseFirstAidKit()
{
	if (bCanUseFirstAidKit == false || Health == 100.f)
		return;

	FItemStruct* FirstAidKitItem = InventoryComponent->GetItemFromInventory(FirstAidKitRowName);
	if (FirstAidKitItem == nullptr)
		return;

	FirstAidKitItem->Item_Amount--;
	Health += FirstAidKitHealth;
	if (Health > 100.f)
		Health = 100.f;

	if (UseFirstAidKitSound) 
		UGameplayStatics::PlaySound2D(GetWorld(), UseFirstAidKitSound);

	HudWidget->SetHealthBarPercent(Health);
	HudWidget->SetCurrentNumberOfFirstAidKits(FirstAidKitItem->Item_Amount);
	HudWidget->PlayUseFirstAidKitAnim();

	ElementBar ProgressHealBar{ DelayAfterUseFirstAidKit };
	HudWidget->AddElementToProgress(EUseableElement::Heal, ProgressHealBar);
	HudWidget->PlayButtonAnimation(EATP_PressedButton_Heal);

	UpdateAlbertosInventory();

	if (FirstAidKitItem->Item_Amount <= 0)
		InventoryComponent->DeleteItemFromInventory(*FirstAidKitItem);

	bCanUseFirstAidKit = false;
	GetWorldTimerManager().SetTimer(UseFirstAidKitHandle, this, &AMarineCharacter::CanUseFirstAidKit, DelayAfterUseFirstAidKit, false);
}
#pragma endregion 

#pragma region ///////////////////////////// TAKE/DROP ITEM ////////////////////////////
void AMarineCharacter::KeyEPressed()
{
	if (WidgetInteractionComponent->IsOverInteractableWidget())
	{
		WidgetInteractionComponent->PressPointerKey(EKeys::LeftMouseButton);
	}
	else TakeAndDropComponent->Take();
}

void AMarineCharacter::KeyEReleased()
{
	WidgetInteractionComponent->ReleasePointerKey(EKeys::LeftMouseButton);
}
#pragma endregion 

#pragma region //////////////////////////////// DAMAGE /////////////////////////////////
void AMarineCharacter::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (bIsDead == true)
		return;

	if (MarineHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MarineHitSound, NewHit.ImpactPoint);

	if (NewSphereRadius != 0.f && IsValid(BulletActor))
	{
		CapsulePawn->AddRadialImpulse(BulletActor->GetActorLocation(), NewSphereRadius, NewImpulseForce, ERadialImpulseFalloff::RIF_Linear, true);
		Health -= NewDamage / 15;
	}
	else Health -= NewDamage;

	if (IsValid(HudWidget) == true)
	{
		HudWidget->SetHealthBarPercent(Health);
		HudWidget->PlayGotDamageAnim();
	}

	if (Health <= 0.f)
	{
		bIsDead = true;
		Health = 0.f;
		SpawnDeathWidgetComponent->SpawnDeathWidget(MarinePlayerController);
	}
}
#pragma endregion 

#pragma region //////////////////////////////// WIDGETS ////////////////////////////////
void AMarineCharacter::MakeHudWidget()
{
	if (HUDClass == nullptr && IsValid(MarinePlayerController) == false)
		return;

	HudWidget = Cast<UHUDWidget>(CreateWidget(MarinePlayerController, HUDClass));
	HudWidget->AddToViewport();
}

void AMarineCharacter::MakeCrosshire(bool bShouldRemoveFromParent)
{
	if (IsValid(CrosshairWidget))
	{
		if (bShouldRemoveFromParent == true)
		{
			CrosshairWidget->RemoveFromParent();
			CrosshairWidget = nullptr;
		}
		return;
	}

	if (CrosshairClass == nullptr && IsValid(MarinePlayerController) == false)
		return;

	CrosshairWidget = CreateWidget(MarinePlayerController, CrosshairClass);
	CrosshairWidget->AddToViewport();
}

void AMarineCharacter::UpdateHudWidget()
{
	WeaponHandlerComponent->UpdateWeaponInformationOnHud();

	FItemStruct* FirstAidKitItem = InventoryComponent->GetItemFromInventory(FirstAidKitRowName);
	HudWidget->SetCurrentNumberOfFirstAidKits(FirstAidKitItem ? FirstAidKitItem->Item_Amount : 0);
}
#pragma endregion 

#pragma region //////////////////////////////// ALBERTO ////////////////////////////////
void AMarineCharacter::UpdateAlbertosInventory(bool bShouldUpdateInventory, bool bShouldUpdateCrafting)
{
	if (AlbertoPawn) CraftingWidget = Cast<UCraftingAlbertosWidget>(AlbertoPawn->GetCraftingTableWidget());
	if (CraftingWidget == nullptr) return;

	if (bShouldUpdateCrafting == true)
	{
		CraftingWidget->SetRecipesData(this);
		CraftingWidget->SwitchCurrentCraftingItem();
	}

	if (bShouldUpdateInventory == true)
	{
		CraftingWidget->AddItemToTileView(InventoryComponent->Inventory_Items);
	}
}

void AMarineCharacter::CallAlbertosPressed()
{
	if (IsValid(AlbertoPawn) == false)
		return;

	AlbertoPawn->CallAlbertoToThePlayer(GetActorLocation());
}

#pragma endregion 

#pragma region ////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////
void AMarineCharacter::MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings)
{
	CroachAndSlideComponent->CrouchReleased();
	JumpComponent->TurnOffJump(bShouldCancelGameplayThings);

	if (!bShouldCancelGameplayThings) return;

	if (WeaponHandlerComponent->GetIsPlayerInAds()) WeaponHandlerComponent->ADSReleased();
}

// When the player does not move the mouse during the pawn's spawn, the rotation of the RootComponent is not updated and, 
// because of this, the RootComponent Forward vector may point in the wrong direction
void AMarineCharacter::ReplaceRootComponentRotation()
{
	FRotator RootCompRot = MarinePlayerController->GetRootComponent()->GetComponentRotation();
	RootCompRot.Yaw = GetActorRotation().Yaw;
	MarinePlayerController->GetRootComponent()->SetWorldRotation(RootCompRot);
}

bool AMarineCharacter::MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult& OutHitResult, bool bDebug)
{
	if (bDebug) DrawDebugBox(GetWorld(), NewStart, FVector(Size), FColor::Red, true);
	return GetWorld()->SweepSingleByChannel(OutHitResult, NewStart, NewEnd, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(Size));
}
#pragma endregion 

#pragma region //////////// GETTERS //////////////////
bool AMarineCharacter::GetIsWallrunning() const
{
	return WallrunComponent->GetIsWallrunning();
}

bool AMarineCharacter::GetIsPlayerLerpingToHookLocation() const
{
	return SwingComponent->GetIsPlayerLerpingToHookPosition();
}

bool AMarineCharacter::GetIsInSlowMotion() const
{
	return SlowMotionComponent->GetIsInSlowMotion();
}

bool AMarineCharacter::GetIsCrouching() const
{
	return CroachAndSlideComponent->GetIsCrouching();
}

FVector AMarineCharacter::GetCameraLocation() const
{
	return Camera->GetComponentLocation();
}

bool AMarineCharacter::GetIsMessageDisplayed() const
{
	return MessageHandlerComponent->GetIsMessageDisplayed();
}

bool AMarineCharacter::GetIsInPauseMenu() const
{
	return PauseMenuComponent->GetIsInPauseMenu();
}

bool AMarineCharacter::GetIsInAir() const
{
	return JumpComponent->GetIsInAir();
}
#pragma endregion 