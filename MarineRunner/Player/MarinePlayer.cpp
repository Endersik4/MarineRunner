// Copyright Adam Bartela.All Rights Reserved

#include "MarinePlayer.h"
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
#include "Perception/AISense_Hearing.h"

#include "MarineRunner/Player/MarinePlayerController.h"
#include "MarineRunner/Player/Components/Movement/JumpComponent.h"
#include "MarineRunner/Player/Components/Movement/CroachAndSlideComponent.h"
#include "MarineRunner/Player/Components/Movement/DashComponent.h"
#include "MarineRunner/Player/Components/Movement/WallrunComponent.h"
#include "MarineRunner/Player/Components/Movement/SlowMotionComponent.h"
#include "MarineRunner/Player/Components/Movement/PullUpComponent.h"
#include "MarineRunner/Player/Components/Movement/SwingComponent.h"
#include "MarineRunner/Player/Components/TakeComponent.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/Components/DeathComponent.h"
#include "MarineRunner/Player/Components/MessageHandlerComponent.h"
#include "MarineRunner/Player/SaveLoadGame/SaveLoadPlayerComponent.h"
#include "MarineRunner/Player/Components/ArmsSwayComponent.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "MarineRunner/Player/GameMenu/PauseMenu/PauseMenuComponent.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/Widgets/CrosshairWidget.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/Components/AlbertosToPlayerComponent.h"
#include "MarineRunner/Albertos/Widgets/Crafting/CraftingAlbertosWidget.h"

// Sets default values
AMarineCharacter::AMarineCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsulePawn = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Pawn Component"));
	CapsulePawn->SetCapsuleHalfHeight(180.f);
	CapsulePawn->SetCapsuleRadius(70.f);
	RootComponent = CapsulePawn;

	CapsulePawn->SetSimulatePhysics(true);
	CapsulePawn->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	CapsulePawn->SetCollisionProfileName(FName(TEXT("PlayerCapsule")));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CapsulePawn);
	Camera->bUsePawnControlRotation = true;

	GroundLocationSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Ground Location Scene Component"));
	GroundLocationSceneComponent->SetupAttachment(RootComponent);
	RoofLocationSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Roof Location Scene Component"));
	RoofLocationSceneComponent->SetupAttachment(RootComponent);

	ArmsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms Skeletal Mesh"));
	ArmsSkeletalMesh->SetupAttachment(Camera);

	JumpComponent = CreateDefaultSubobject<UJumpComponent>(TEXT("JumpComponent"));
	CroachAndSlideComponent = CreateDefaultSubobject<UCroachAndSlide>(TEXT("CroachAndSlideComponent"));

	TakeAndDropComponent = CreateDefaultSubobject<UTakeAndDrop>(TEXT("Take Component"));

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

	if (ensureMsgf(IsValid(GetController()), TEXT("Player Controller is nullptr in MarinePlayer!")))
	{
		MarinePlayerController = Cast<AMarinePlayerController>(GetController());
	}

	PauseMenuComponent->ChangeUIToGameOnly();

	MakeCrosshire();
	MakeHudWidget();

	UpdateHudWidget();

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

	PlayerInputComponent->BindAction(TEXT("PrimaryAction"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::PrimaryAction);
	PlayerInputComponent->BindAction(TEXT("PrimaryAction"), IE_Released, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::ReleasedPrimaryAction);

	PlayerInputComponent->BindAction(TEXT("SecondaryAction"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::SecondaryAction);
	PlayerInputComponent->BindAction(TEXT("SecondaryAction"), IE_Released, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::ReleasedSecondaryAction);
	
	PlayerInputComponent->BindAxis(TEXT("TertiaryAction"), WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::TertiaryAction);

	PlayerInputComponent->BindAction(TEXT("ActionFromKey_One"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::ActionFromKey_One);

	PlayerInputComponent->BindAction(TEXT("Drop"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::DropCurrentHoldingWeapon);
	PlayerInputComponent->BindAction(TEXT("HideWeapon"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::CallHideWeaponByPlayer);

	//Weapon Inventory
	PlayerInputComponent->BindAction<FSelectWeaponDelegate>(TEXT("First_Weapon"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::SelectWeaponFromQuickInventory, 1);
	PlayerInputComponent->BindAction<FSelectWeaponDelegate>(TEXT("Second_Weapon"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::SelectWeaponFromQuickInventory, 2);
	PlayerInputComponent->BindAction<FSelectWeaponDelegate>(TEXT("Third_Weapon"), IE_Pressed, WeaponHandlerComponent.Get(), &UWeaponHandlerComponent::SelectWeaponFromQuickInventory, 3);


	PlayerInputComponent->BindAction(TEXT("FirstAidKit"), IE_Pressed, this, &AMarineCharacter::UseFirstAidKit);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AMarineCharacter::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AMarineCharacter::Right);

	//Take
	PlayerInputComponent->BindAction(TEXT("Take"), IE_Pressed, this, &AMarineCharacter::TakePressed);
	PlayerInputComponent->BindAction(TEXT("Take"), IE_Released, this, &AMarineCharacter::TakeReleased);

	//Movement
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, JumpComponent.Get(), &UJumpComponent::Jump);
	PlayerInputComponent->BindAction<FCrouchSlideDelegate>(TEXT("Crouch"), IE_Pressed, CroachAndSlideComponent.Get(), &UCroachAndSlide::CrouchPressed, false);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, CroachAndSlideComponent.Get(), &UCroachAndSlide::CrouchReleased);

	PlayerInputComponent->BindAction<FCrouchSlideDelegate>(TEXT("Slide"), IE_Pressed, CroachAndSlideComponent.Get(), &UCroachAndSlide::CrouchPressed, true);
	PlayerInputComponent->BindAction(TEXT("Slide"), IE_Released, CroachAndSlideComponent.Get(), &UCroachAndSlide::CrouchReleased);

	//Gameplay components
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, DashComponent.Get(), &UDashComponent::Dash);
	PlayerInputComponent->BindAction(TEXT("Swing"), IE_Pressed, SwingComponent.Get(), &USwingComponent::SwingPressed);
	PlayerInputComponent->BindAction(TEXT("SlowMotion"), IE_Pressed, SlowMotionComponent.Get(), &USlowMotionComponent::TurnOnSlowMotion);

	// Menu
	FInputActionBinding& MainMenuToggle = PlayerInputComponent->BindAction(TEXT("MainMenu"), IE_Pressed, PauseMenuComponent.Get(), &UPauseMenuComponent::PauseGame);
	MainMenuToggle.bExecuteWhenPaused = true;

	FInputActionBinding& CloseMessageToggle = PlayerInputComponent->BindAction<FCloseMessageDelegate>(TEXT("CloseMessage"), IE_Pressed, MessageHandlerComponent.Get(), &UMessageHandlerComponent::DeleteCurrentDisplayedMessage, this);
	CloseMessageToggle.bExecuteWhenPaused = true;

	// You Died Bindings
	FInputActionBinding& YouDiedRestartToggle = PlayerInputComponent->BindAction(TEXT("RestartGameWhenDead"), IE_Pressed, SpawnDeathWidgetComponent.Get(), &USpawnDeathWidgetComponent::RestartGameInYouDiedWidget);
	YouDiedRestartToggle.bExecuteWhenPaused = true;
	FInputActionBinding& YouDiedQuitToggle = PlayerInputComponent->BindAction(TEXT("QuitGameWhenDead"), IE_Pressed, SpawnDeathWidgetComponent.Get(), &USpawnDeathWidgetComponent::QuitGameInYouDiedWidget);
	YouDiedQuitToggle.bExecuteWhenPaused = true;

	PlayerInputComponent->BindAction(TEXT("CallAlbertos"), IE_Pressed, this, &AMarineCharacter::CallAlbertosPressed);
}

void AMarineCharacter::ChangeMouseSensitivity(const FSettingSavedInJsonFile& NewMouseSensitivity, bool bResetMouseSensitivity)
{
	if (!IsValid(MarinePlayerController))
		return;

	if (bResetMouseSensitivity)
	{
		MarinePlayerController->SetMouseSensitivity(OriginalMouseSensitivityJSON);
		return;
	}

	MarinePlayerController->SetMouseSensitivity(NewMouseSensitivity);
}

void AMarineCharacter::LoadFieldOfViewFromSettings()
{
	if (!IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(GameInstance))
		return;

	GameInstance->FindSavedValueAccordingToName(FieldOfViewJSON.FieldName, FieldOfViewJSON.FieldValue);
	Camera->SetFieldOfView(FieldOfViewJSON.FieldValue);
}

#pragma region //////////////////////////////// MOVEMENT ///////////////////////////////
void AMarineCharacter::Forward(float Axis)
{
	if (!IsValid(MarinePlayerController))
		return;

	// There is a bug where ForwardVector is not correct and had to use Controller->RootComponent but this way have other bug, when player looks up or down then he cant 
	// go forward/backward and to fix this i had to rotate RightVector by -90.f
	FVector Dir = MarinePlayerController->GetRootComponent()->GetRightVector().RotateAngleAxis(DegreeForForwardVector, FVector(0.f, 0.f, 1.f));
	
	if (bConstantlyGoForward)
		Axis = 1.f;
	if (GetIsWallrunning())
		Dir = WallrunComponent->GetWallrunDirection();

	Move(Dir, Axis, FName(TEXT("Right")));
}

void AMarineCharacter::Right(float Axis)
{
	if (!IsValid(MarinePlayerController))
		return;

	Move(MarinePlayerController->GetRootComponent()->GetRightVector(), Axis, FName(TEXT("Forward")));
}

void AMarineCharacter::Move(FVector Direction, float Axis, const FName InputAxisName)
{
	PlayFootstepsSound();

	if (GetIsWallrunning())
		return;

	float TempMovementSpeed = MovementSpeed;
	if (GetIsInSlowMotion())
		TempMovementSpeed = SlowMotionComponent->GetMovementSpeedInSlowMotion();
	if (bIsInCutscene)
		TempMovementSpeed = MovementSpeedInCutscene;

	float Speed = (TempMovementSpeed / MovementForceDividerWhenInADS) / (GetInputAxisValue(InputAxisName) != 0.f ? ForwardAndRightAtTheSameTimeDivider : 1);

	if (GetIsWallrunning())
		Speed = (MovementSpeed / MovementForceDividerWhenInADS) * MovementSpeedMutliplier;

	if (GetIsInAir() && !GetIsWallrunning() )
		Speed /= (JumpComponent->GetDividerForMovementWhenInAir() / UGameplayStatics::GetGlobalTimeDilation(GetWorld()));

	Direction.Z = 0.f;
	const FVector& Force = (Axis * Direction * Speed) + CalculateCounterMovement();
	CapsulePawn->AddImpulse(Force);
}
FVector AMarineCharacter::CalculateCounterMovement()
{
	const FVector& CounterVelocity = GetVelocity() * FVector(-1.f, -1.f, 0.f);

	float CounterForce = CounterMovementForce;
	if (GetIsInSlowMotion())
		CounterForce = SlowMotionComponent->GetCounterForceInSlowMotion();

	if (GetIsInAir() && !GetIsWallrunning())
	{
		float CounterMovementAccordingToSpeed = GetVelocity().Length() > MaxVelocityForStaticCounterMovement ? GetVelocity().Length() / MaxVelocityForStaticCounterMovement : 1.f;
		CounterForce = CounterForce / (JumpComponent->GetDividerForCounterForceWhenInAir() * CounterMovementAccordingToSpeed);
	
		if (GetIsInSlowMotion())
			CounterForce *= SlowMotionComponent->GetCounterForceMultiplierWhenInAir();
	}

	return FVector(CounterForce * CounterVelocity.X, CounterForce * CounterVelocity.Y, 0);
}
#pragma endregion

#pragma region //////////////////////////// FOOTSTEPS SOUND ////////////////////////////
bool AMarineCharacter::CheckIfCanPlayFootstepsSound()
{
	if (!bCanPlayFootstepsSound)
		return false;

	if (!WallrunComponent->GetIsWallrunning() && GetIsInAir())
		return false;

	if (CroachAndSlideComponent->GetIsSliding())
		return false;

	if ((GetVelocity().Length() >= VelocityRangeToDisableFootsteps.GetLowerBoundValue()
		&& GetVelocity().Length() <= VelocityRangeToDisableFootsteps.GetUpperBoundValue()))
		return false;

	if (GetInputAxisValue(TEXT("Forward")) == 0.f && GetInputAxisValue(TEXT("Right")) == 0.f && !WallrunComponent->GetIsWallrunning())
		return false;

	return true;
}

void AMarineCharacter::PlayFootstepsSound()
{
	if (!CheckIfCanPlayFootstepsSound())
		return;

	float TimeToPlayNextStep = NormalTimeBetweenNextStep;
	if (WallrunComponent->GetIsWallrunning())
	{
		TimeToPlayNextStep = WallrunTimeBetweenNextStep;

		if (IsValid(FootstepsWallrunSound))
			UGameplayStatics::PlaySound2D(GetWorld(), FootstepsWallrunSound);
		else
			UE_LOG(LogTemp, Warning, TEXT("Footsteps Wallrun Sound is nullptr in MarinePlayer!"));
	}
	else if (GetIsCrouching())
	{
		TimeToPlayNextStep = CrouchTimeBetweenNextStep;

		if (IsValid(FootstepsCrouchSound))
			UGameplayStatics::SpawnSoundAttached(FootstepsCrouchSound, GroundLocationSceneComponent);
		else
			UE_LOG(LogTemp, Warning, TEXT("Footsteps Crouch Sound is nullptr in MarinePlayer!"));
	}
	else
	{
		if (IsValid(FootstepsSound))
			UGameplayStatics::SpawnSoundAttached(FootstepsSound, GroundLocationSceneComponent);
		else
			UE_LOG(LogTemp, Warning, TEXT("Footsteps Sound is nullptr in MarinePlayer!"));
	}

	if (bIsInCutscene)
		TimeToPlayNextStep = TimeBetweenNextStepInCutscene;

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), FootstepsSoundLoudnessForEnemy, this, FootstepsSoundMaxRangeForEnemy);

	bCanPlayFootstepsSound = false;
	TimeToPlayNextStep *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	GetWorldTimerManager().SetTimer(FootstepsHandle, this, &AMarineCharacter::SetCanPlayFootstepsSound, TimeToPlayNextStep, false);
}

#pragma endregion 

#pragma region ///////////////////////////// FIRST AID KIT /////////////////////////////

void AMarineCharacter::UseFirstAidKit()
{
	if (!bCanUseFirstAidKit || Health == OriginalHealth || bIsInCutscene)
		return;

	FItemStruct* FirstAidKitItem = InventoryComponent->GetItemFromInventory(FirstAidKitRowName);
	if (!FirstAidKitItem)
		return;

	FirstAidKitItem->Item_Amount--;
	Health += FirstAidKitHealth;
	if (Health > OriginalHealth)
		Health = OriginalHealth;

	if (IsValid(UseFirstAidKitSound)) 
		UGameplayStatics::PlaySound2D(GetWorld(), UseFirstAidKitSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Use First Aid Kit Sound is nullptr in MarinePlayer!"));

	if (IsValid(HudWidget))
	{
		HudWidget->PlayUseFirstAidKitAnim();
		HudWidget->UpdateHealthBarPercent(Health);
		HudWidget->UpdateCurrentNumberOfFirstAidKits(FirstAidKitItem->Item_Amount);

		FPowerUpLoaded HealthDelay = FPowerUpLoaded(true, DelayAfterUseFirstAidKit, HudWidget->ActiveHealAnim, HudWidget->HealBar);
		HudWidget->AddNewPowerUpToStartLoading(HealthDelay);
		HudWidget->PlayButtonAnimation(EATP_PressedButton_Heal);
	}

	UpdateAlbertosInventory();

	if (FirstAidKitItem->Item_Amount <= 0)
		InventoryComponent->DeleteItemFromInventory(*FirstAidKitItem);

	bCanUseFirstAidKit = false;
	GetWorldTimerManager().SetTimer(UseFirstAidKitHandle, this, &AMarineCharacter::CanUseFirstAidKit, DelayAfterUseFirstAidKit, false);
}
#pragma endregion 

#pragma region ///////////////////////////// TAKE ////////////////////////////
void AMarineCharacter::TakePressed()
{
	if (bIsInCutscene)
		return;

	if (WidgetInteractionComponent->IsOverInteractableWidget())
	{
		WidgetInteractionComponent->PressPointerKey(EKeys::LeftMouseButton);
	}
	else if (IsValid(TakeAndDropComponent))
	{
		TakeAndDropComponent->Take();
	}
}

void AMarineCharacter::TakeReleased()
{
	WidgetInteractionComponent->ReleasePointerKey(EKeys::LeftMouseButton);

	if (IsValid(TakeAndDropComponent))
		TakeAndDropComponent->TakeReleased();
}
#pragma endregion 

#pragma region //////////////////////////////// DAMAGE /////////////////////////////////
void AMarineCharacter::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (bIsDead)
		return;

	if (IsValid(MarineHitSound)) 
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MarineHitSound, NewHit.ImpactPoint);
	else
		UE_LOG(LogTemp, Warning, TEXT("Marine Hit Sound is nullptr in MarinePlayer!"));

	// if NewSphereRadius != 0 then bullet with sphere radial damage was used as damage type
	if (NewSphereRadius != 0.f && IsValid(BulletActor))
	{
		CapsulePawn->AddRadialImpulse(BulletActor->GetActorLocation(), NewSphereRadius, NewImpulseForce, ERadialImpulseFalloff::RIF_Linear, true);
		Health -= NewDamage / DividerForRadialDamage;
	}
	else 
	{
		Health -= NewDamage;
	}

	if (IsValid(HudWidget))
	{
		HudWidget->UpdateHealthBarPercent(Health);
		HudWidget->PlayGotDamageAnim();
	}

	if (Health <= 0.f)
		PlayerDead();
}

void AMarineCharacter::PlayerDead()
{
	bIsDead = true;
	Health = 0.f;
	SpawnDeathWidgetComponent->SpawnDeathWidget(MarinePlayerController);

	if (!IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
		return;

	TObjectPtr<UMarineRunnerGameInstance> MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(MarineRunnerGameInstance))
		return;

	MarineRunnerGameInstance->ResetDetectedEnemy();
}
#pragma endregion 

#pragma region //////////////////////////////// WIDGETS ////////////////////////////////
void AMarineCharacter::MakeHudWidget()
{
	if (!HUDClass || !IsValid(MarinePlayerController))
		return;

	HudWidget = Cast<UHUDWidget>(CreateWidget(MarinePlayerController, HUDClass));

	if (!HudWidget)
		return;
	HudWidget->AddToViewport();

	SaveLoadPlayerComponent->SpawnNewGameWidget();
	SaveLoadPlayerComponent->LoadHudVariables();
}

void AMarineCharacter::MakeCrosshire(bool bShouldRemoveFromParent)
{
	if (IsValid(SpawnedCrosshairWidget))
	{
		if (bShouldRemoveFromParent)
		{
			SpawnedCrosshairWidget->RemoveFromParent();
			SpawnedCrosshairWidget = nullptr;
		}
		return;
	}

	if (!CrosshairWidgetClass || !IsValid(MarinePlayerController))
		return;

	SpawnedCrosshairWidget = Cast<UCrosshairWidget>(CreateWidget(MarinePlayerController, CrosshairWidgetClass));

	if (!IsValid(SpawnedCrosshairWidget))
		return;

	SpawnedCrosshairWidget->AddToViewport();
}

void AMarineCharacter::UpdateHudWidget()
{
	WeaponHandlerComponent->UpdateWeaponInformationOnHud();

	if (!IsValid(HudWidget))
		return;

	HudWidget->UpdateHealthBarPercent(Health);

	FItemStruct* FirstAidKitItem = InventoryComponent->GetItemFromInventory(FirstAidKitRowName);
	if (FirstAidKitItem)
	{
		HudWidget->UpdateCurrentNumberOfFirstAidKits(FirstAidKitItem->Item_Amount > MaxAmountOfFirstAidKitsOnHud ? MaxAmountOfFirstAidKitsOnHud : FirstAidKitItem->Item_Amount);
	}
	else
	{
		HudWidget->UpdateCurrentNumberOfFirstAidKits(0);
	}
}

void AMarineCharacter::ShowHUD(bool bShow)
{
	if (IsValid(HudWidget))
		HudWidget->ShowHUDWithAnim(bShow);
	if (IsValid(SpawnedCrosshairWidget))
		SpawnedCrosshairWidget->ShowCrosshairWithAnim(bShow);
}
#pragma endregion 

#pragma region //////////////////////////////// ALBERTO ////////////////////////////////
void AMarineCharacter::UpdateAlbertosInventory(bool bShouldUpdateInventory, bool bShouldUpdateCrafting)
{
	if (!IsValid(AlbertoPawn))
		return;
	
	CraftingWidget = Cast<UCraftingAlbertosWidget>(AlbertoPawn->GetCraftingTableWidget());
	if (!IsValid(CraftingWidget)) 
		return;

	if (bShouldUpdateInventory)
	{
		CraftingWidget->AddItemsToInventoryTileView(InventoryComponent->Inventory_Items);
	}

	if (bShouldUpdateCrafting)
	{
		InventoryComponent->MoveWeaponRecipesToEndQueue();
		CraftingWidget->SetPlayer(this);
		CraftingWidget->SetRecipesData(InventoryComponent->Items_Recipes);
		CraftingWidget->SwitchCurrentCraftingItem();
	}
}

void AMarineCharacter::CallAlbertosPressed()
{
	if (!IsValid(AlbertoPawn) || bIsInCutscene)
		return;
	
	if (!IsValid(AlbertoPawn->GetAlbertosToPlayerComponent()))
		return;

	AlbertoPawn->GetAlbertosToPlayerComponent()->CallAlbertosToThePlayer(GetActorLocation());
}

#pragma endregion 

// When the player does not move the mouse during the pawn's spawn, the rotation of the RootComponent is not updated and, 
// because of this, the RootComponent Forward vector may point in the wrong direction
void AMarineCharacter::ReplaceRootComponentRotation()
{
	if (!IsValid(MarinePlayerController))
		return;

	FRotator RootCompRot = MarinePlayerController->GetRootComponent()->GetComponentRotation();
	RootCompRot.Yaw = GetActorRotation().Yaw;
	MarinePlayerController->GetRootComponent()->SetWorldRotation(RootCompRot);
}

#pragma region //////////// GETTERS //////////////////
bool AMarineCharacter::GetIsWallrunning() const
{
	return WallrunComponent->GetIsWallrunning();
}

bool AMarineCharacter::GetIsPlayerMovingToHookLocation() const
{
	return SwingComponent->GetIsPlayerMovingToHookPosition();
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

bool AMarineCharacter::GetIsPullingUp() const
{
	return PullUpComponent->GetIsPullingUp();
}
#pragma endregion 