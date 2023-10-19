// Copyright Adam Bartela.All Rights Reserved


#include "MarineCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetInteractionComponent.h"
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
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"
#include "MarineRunner/MarinePawnClasses/SlowMotionComponent.h"
#include "MarineRunner/MarinePawnClasses/PullUpComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/SwingComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Widgets/Menu/GameSavedNotificationWidget.h"
#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "MarineRunner/Objects/Checkpoint.h"


// Sets default values
AMarineCharacter::AMarineCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsulePawn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CapsulePawn"));
	RootComponent = CapsulePawn;

	CapsulePawn->SetSimulatePhysics(true);
	//CapsulePawn->SetMassScale(NAME_None, 2.f);
	CapsulePawn->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	CapsulePawn->SetCastShadow(false);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CapsulePawn);
	Camera->bUsePawnControlRotation = true;

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
	
	LoadSavedSettingsFromGameInstance();

	MakeCrosshire();
	MakeHudWidget();
	CopyOfOriginalForce = MovementForce;
	LoadGame();

	if (IsValid(HudWidget))
	{
		HudWidget->SetHealthBarPercent(Health);
		HudWidget->SetCurrentNumberOfFirstAidKits(GetInventoryComponent()->Inventory_Items.Find(GetFirstAidKitName())->Item_Amount);
	}
}

// Called every frame
void AMarineCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	JumpTick(DeltaTime);
	CheckIfIsInAir();
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
	PlayerInputComponent->BindAction(TEXT("Drop"), IE_Pressed, TakeAndDropComponent, &UTakeAndDrop::DropItem);

	//Movement
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMarineCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, CroachAndSlideComponent, &UCroachAndSlide::CrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, CroachAndSlideComponent, &UCroachAndSlide::CrouchReleased);

	//Gameplay components
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, DashComponent, &UDashComponent::Dash);
	PlayerInputComponent->BindAction(TEXT("Swing"), IE_Pressed, SwingComponent, &USwingComponent::SwingPressed);
	PlayerInputComponent->BindAction(TEXT("SlowMotion"), IE_Pressed, SlowMotionComponent, &USlowMotionComponent::TurnOnSlowMotion);

	// Menu
	FInputActionBinding& MainMenuToggle = PlayerInputComponent->BindAction(TEXT("MainMenu"), IE_Pressed, PauseMenuComponent, &UPauseMenuComponent::PauseGame);
	MainMenuToggle.bExecuteWhenPaused = true;

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
	const float DegreeForForwardVector = -90.f;
	FVector Dir = MarinePlayerController->GetRootComponent()->GetRightVector().RotateAngleAxis(DegreeForForwardVector, FVector(0.f,0.f,1.f));

	if (WallrunComponent->GetShouldPlayerGoForward() == true)
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
	if (bIsInAir == true && GetIsWallrunning() == false)
	{
		Speed /= DividerForMovementWhenInAir;
	}
	if (SlowMotionComponent->GetIsInSlowMotion() == true)
	{
		if (bIsInAir == true) Speed /= 3.f;
		else Speed *= 2.4f;
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
	if (bIsInAir == true && GetIsWallrunning() == false)
	{
		CounterForce = CounterMovementForce / DividerForMovementWhenInAir;
		if (SlowMotionComponent->GetIsInSlowMotion() == true) CounterForce /= 3.f;
	}

	return FVector(CounterForce * Velocity.X, CounterForce * Velocity.Y, 0);
}
#pragma endregion

#pragma region ////////////////////////////////// JUMP /////////////////////////////////
void AMarineCharacter::Jump()
{
	if (WallrunComponent->GetCanJump() == false) return;

	if (bIsInAir == false || (bDelayIsInAir && bIsJumping == false) || WallrunComponent->ShouldAddImpulseAfterWallrun(true))
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

void AMarineCharacter::DelayJump()
{
	if (bCanDelayJump == false) return;

	Jump();
	bCanDelayJump = false;
	GetWorldTimerManager().ClearTimer(DelayJumpHandle);
}
#pragma endregion 

#pragma region ////////////////////////////////// AIR //////////////////////////////////
void AMarineCharacter::CheckIfIsInAir()
{
	FHitResult Hit;
	//Check if there is ground under the player, if not, the player is in the air
	if (!MakeCheckBox(FVector(25.f,25.f, 2.f), GetActorLocation(), GetActorLocation(), Hit))
	{
		//The first moment a player is in the air
		if (bIsInAir == false)
		{
			bDelayIsInAir = true;
			GetWorldTimerManager().SetTimer(DelayIsInAirHandle, this, &AMarineCharacter::SetDelayIsInAir, DelayIsInAirTime, false);
		}
		bIsInAir = true;
		bIsOnRamp = false;
	}
	else
	{
		//The first moment a player touches the ground
		if (bIsInAir)
		{
			WallrunComponent->CallResetWallrunningAfterLanding();

			IsOnGround = true;

			bIsInAir = false;
			bDelayIsInAir = false;
			PullUpComponent->SetPulledHimselfUp(false);

			if (ImpactOnFloorSound) UGameplayStatics::SpawnSoundAttached(ImpactOnFloorSound, CapsulePawn);

			if (CroachAndSlideComponent->GetIsCrouching() == false) MovementForce = CopyOfOriginalForce;
		}

		if (Hit.GetActor()->ActorHasTag("Ramp"))
		{
			bIsOnRamp = true;
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
		else bIsOnRamp = false;

		DelayJump();
		
	}
}

#pragma endregion

#pragma region //////////////////////////// FOOTSTEPS SOUND ////////////////////////////
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

	FirstAidKitItem = InventoryComponent->Inventory_Items.Find(FirstAidKits_Name);
	if (FirstAidKitItem == nullptr) 
		return;
	if (FirstAidKitItem->Item_Amount <= 0) 
		return;

	FirstAidKitItem->Item_Amount--;
	Health += FirstAidKitHealth;
	if (Health > 100.f) Health = 100.f;

	if (UseFirstAidKitSound) UGameplayStatics::SpawnSound2D(GetWorld(), UseFirstAidKitSound);

	HudWidget->SetHealthBarPercent(Health);
	HudWidget->SetCurrentNumberOfFirstAidKits(GetInventoryComponent()->Inventory_Items.Find(GetFirstAidKitName())->Item_Amount);
	HudWidget->PlayUseFirstAidKitAnim();

	ElementBar ProgressHealBar{ DelayAfterUseFirstAidKit };
	HudWidget->AddElementToProgress(EUseableElement::Heal, ProgressHealBar);
	HudWidget->PlayButtonAnimation(EATP_PressedButton_Heal);

	UpdateAlbertosInventory();

	bCanUseFirstAidKit = false;
	GetWorldTimerManager().SetTimer(FirstAidKitHandle, this, &AMarineCharacter::CanUseFirstAidKit, DelayAfterUseFirstAidKit, false);
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

#pragma region /////////////////////////////// SAVE/LOAD ///////////////////////////////
void AMarineCharacter::SaveGame(AActor* JustSavedCheckpoint)
{
	CurrentSaveGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));

	CurrentSaveGameInstance->PrepareSaveGame();
	CurrentSaveGameInstance->CopySaveInfoToCurrentGameInstance(GetWorld());

	CurrentSaveGameInstance->SaveGame(Health, WeaponHandlerComponent->GetGun(), WeaponInventoryComponent->ReturnAllWeapons(), InventoryComponent->Inventory_Items);
	CurrentSaveGameInstance->CurrentCheckpoint = JustSavedCheckpoint;

	CurrentSaveGameInstance->SavedPlayerLocation = GetActorLocation();
	CurrentSaveGameInstance->SavedPlayerRotation = GetActorRotation();

	CurrentSaveGameInstance->MakeJsonFileWithSaveInfo(MarinePlayerController, UGameplayStatics::GetCurrentLevelName(GetWorld()));

	UGameplayStatics::SaveGameToSlot(CurrentSaveGameInstance, CurrentSaveGameInstance->GetSaveGameName() +"/"+ CurrentSaveGameInstance->GetSaveGameName(), 0);

	SpawnPassingWidget(GameSavedNotificationWidgetClass);
}

void AMarineCharacter::LoadGame()
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FString SlotName = IsValid(GameInstance) == false ? "MySlot" : GameInstance->SlotSaveGameNameToLoad;
	SlotName += "/" + SlotName;

	USaveMarineRunner* LoadGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));
	if (!UGameplayStatics::LoadGameFromSlot(SlotName, 0))
	{
		return;
	}

	LoadGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	SetActorLocation(LoadGameInstance->SavedPlayerLocation);
	if (IsValid(MarinePlayerController) == true)
		MarinePlayerController->SetControlRotation(LoadGameInstance->SavedPlayerRotation);	

	ACheckpoint* LoadedCheckpoint = Cast<ACheckpoint>(LoadGameInstance->CurrentCheckpoint);
	if (IsValid(LoadedCheckpoint))
	{
		LoadedCheckpoint->DisableCheckpoint();
	}

	LoadGameInstance->LoadGame(this, GameInstance);
}

void AMarineCharacter::LoadSavedSettingsFromGameInstance()
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance) == false || IsValid(MarinePlayerController) == false)
		return;

	const FSettingSavedInJsonFile& CurrentMouseSensName = MarinePlayerController->GetMouseSensitivity();
	GameInstance->SetValueBySavedSettingName(MouseSensitivityJSON.FieldName, MouseSensitivityJSON.FieldValue);
	if (CurrentMouseSensName == MouseSensitivityJSON) ChangeMouseSensitivity(MouseSensitivityJSON);
}

void AMarineCharacter::SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn)
{
	if (IsValid(MarinePlayerController) == false) return;

	UGameSavedNotificationWidget* SpawnedWidget = Cast<UGameSavedNotificationWidget>(CreateWidget(MarinePlayerController, WidgetClassToSpawn));
	if (IsValid(SpawnedWidget) == false)
		return;

	SpawnedWidget->AddToViewport();
}

bool AMarineCharacter::CanPlayerSaveGame()
{
	return bIsInAir || WeaponHandlerComponent->GetIsPlayerInAds() ? false : true;
}

#pragma endregion 

#pragma region //////////////////////////////// DAMAGE /////////////////////////////////
void AMarineCharacter::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (MarineHitSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MarineHitSound, NewHit.ImpactPoint);

	if (NewSphereRadius != 0.f)
	{
		CapsulePawn->AddRadialImpulse(BulletActor->GetActorLocation(), NewSphereRadius, NewImpulseForce, ERadialImpulseFalloff::RIF_Linear, true);
		Health -= NewDamage / 15;
	}
	else Health -= NewDamage;

	if (Health < 0.f)
	{
		Health = 0.f;
		UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
	}

	if (IsValid(HudWidget) == false) return;

	HudWidget->SetHealthBarPercent(Health);
	HudWidget->PlayGotDamageAnim();
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

	HudWidget->SetCurrentNumberOfFirstAidKits(GetInventoryComponent()->Inventory_Items.Find(GetFirstAidKitName())->Item_Amount);
}
#pragma endregion 

#pragma region //////////////////////////////// ALBERTO ////////////////////////////////
void AMarineCharacter::UpdateAlbertosInventory(bool bShouldUpdateInventory, bool bShouldUpdateCrafting)
{
	if (AlbertoPawn) CraftingWidget = Cast<UCraftingAlbertosWidget>(AlbertoPawn->GetCraftingTableWidget());
	if (CraftingWidget == nullptr) return;
	
	if (bShouldUpdateCrafting == true)
	{
		CraftingWidget->SetRecipesData();
		CraftingWidget->SwitchCurrentCraftingItem();
	}

	if (bShouldUpdateInventory  == true)
	{
		TArray<FItemStruct> ItemDataArray;
		InventoryComponent->Inventory_Items.GenerateValueArray(ItemDataArray);
		CraftingWidget->AddItemToTileView(ItemDataArray);
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
	bIsJumping = false;
	CroachAndSlideComponent->CrouchReleased();

	if (!bShouldCancelGameplayThings) return;

	if (bCanDelayJump) DelayJump();
	if (WeaponHandlerComponent->GetIsPlayerInAds()) WeaponHandlerComponent->ADSReleased();
}

void AMarineCharacter::SetQuickSelect(TMap < int32, AGun* > NewWeaponsStorage)
{
	WeaponInventoryComponent->SetWeaponsStorage(NewWeaponsStorage);
}

bool AMarineCharacter::MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult& OutHitResult, bool bDebug)
{
	if (bDebug) DrawDebugBox(GetWorld(), NewStart, FVector(Size), FColor::Red, true);
	return GetWorld()->SweepSingleByChannel(OutHitResult, NewStart, NewEnd, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(Size));
}

bool AMarineCharacter::MakeCheckLine(FHitResult& OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug, FColor Color)
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

#pragma endregion 