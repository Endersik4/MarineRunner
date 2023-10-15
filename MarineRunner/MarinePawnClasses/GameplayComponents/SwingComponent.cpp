// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SwingComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/Hook.h"
#include "MarineRunner/MarinePawnClasses/SwingLine.h"

// Sets default values for this component's properties
USwingComponent::USwingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USwingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	MarinePlayer = Cast<AMarineCharacter>(GetOwner());
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}


// Called every frame
void USwingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SwingInterp();
	SwingLineCheck();
}

#pragma region ///////////////////////////////// SWING /////////////////////////////////
void USwingComponent::SwingLineCheck()
{
	if (bCanMakeSwingLineCheck == false || bIsPlayerLerpingToHookPosition == true) 
		return;

	FVector LineStart = MarinePlayer->GetCamera()->GetComponentLocation();
	FVector LineEnd = LineStart + (PlayerController->GetRootComponent()->GetForwardVector() * LengthOfSwingLineRaycast);

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResults;

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineStart, LineEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		if (HitResults.GetActor()->ActorHasTag("Hook") == false)
			return;

		AHook* HoveredHook = Cast<AHook>(HitResults.GetActor());

		// Disable previous hook if there was any
		if (IsValid(CurrentFocusedHook) && HoveredHook != CurrentFocusedHook) 
			CurrentFocusedHook->HookActivate(false);
		
		if (HoveredHook->bCanGrabTheHook == false)
			return;

		HoveredHook->HookActivate();
		CurrentFocusedHook = HoveredHook;
		bCanPlayerSwing = true;
	}	
	else if (IsValid(CurrentFocusedHook))
	{
		CurrentFocusedHook->HookActivate(false);
		bCanPlayerSwing = false;
	}
}

//If the player press the Swing button then spawn the Line that is going to the Hook and then wait for SwingDelay to elapsed
void USwingComponent::SwingPressed()
{
	if (bCanPlayerSwing == false || IsValid(CurrentFocusedHook) == false || bIsPlayerLerpingToHookPosition == true || bWasSwingPressed == true)
		return;

	CurrentFocusedHook->StartHookCooldown();
	bWasSwingPressed = true;
	bCanMakeSwingLineCheck = false;

	SpawnSwingEffects();

	GetWorld()->GetTimerManager().SetTimer(SwingHandle, this, &USwingComponent::StartSwingToHook, SwingDelay);
}

void USwingComponent::SpawnSwingEffects()
{
	if (SwingSound) UGameplayStatics::SpawnSound2D(GetWorld(), SwingSound);

	FVector SpawnLocation = MarinePlayer->GetCamera()->GetComponentLocation();
	SpawnLocation += PlayerController->GetRootComponent()->GetForwardVector() * ForwardSwingLineOffset + PlayerController->GetRootComponent()->GetRightVector() * RightSwingLineOffset;
	ASwingLine* SwingLine = GetWorld()->SpawnActor<ASwingLine>(SwingLineClass, SpawnLocation, FRotator(0, 0, 0));

	if (IsValid(SwingLine) == false)
		return;

	SwingLine->SetHookLocation(CurrentFocusedHook->GetActorLocation());
	SwingLine->SetSpeedLine(SwingDelay);
	SwingLine->SetCanTick(true);
}

void USwingComponent::StartSwingToHook()
{
	if (IsValid(CurrentFocusedHook) == false) return;

	MarinePlayer->CapsulePawn->SetPhysicsLinearVelocity(FVector(0));
	bIsPlayerLerpingToHookPosition = true;

	HookLocation = CurrentFocusedHook->GetActorLocation() - HookLocationOffset;

	FVector DirectionOfVector = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(MarinePlayer->GetActorLocation(), HookLocation));
	SwingImpulse = DirectionOfVector * SwingForce;
	MarinePlayer->CapsulePawn->AddImpulse(SwingImpulse);

	//Get Direction To Hook but only in X and Z Axis. Its for Pawn Physics
	SwingImpulse = FRotator(0, SwingImpulse.Rotation().Yaw, 0).Vector();
	//MarinePlayer->SetMovementImpulse(SwingImpulse);

	//Things that cannot happen while Swing
	MarinePlayer->MovementStuffThatCannotHappen();
}

//Interp player to location of the Hook
void USwingComponent::SwingInterp()
{
	if (!bIsPlayerLerpingToHookPosition || IsValid(CurrentFocusedHook) == false) 
		return;

	FVector LocationInterp = FMath::VInterpTo(MarinePlayer->GetActorLocation(), HookLocation, GetWorld()->GetDeltaSeconds(), SwingSpeed);

	StopSwingInterp();

	MarinePlayer->SetActorLocation(LocationInterp);
}
void USwingComponent::StopSwingInterp()
{
	if (MarinePlayer->Camera->GetComponentLocation().Equals(HookLocation, MaxHookDistanceToFinishInterp) == false)
		return;

	bWasSwingPressed = false;
	bIsPlayerLerpingToHookPosition = false;
	bCanMakeSwingLineCheck = true;

	FVector Velocity = MarinePlayer->CapsulePawn->GetPhysicsLinearVelocity() * SwingLinearPhysicsMultiplier;
	Velocity.Z = MarinePlayer->CapsulePawn->GetPhysicsLinearVelocity().Z;
	MarinePlayer->CapsulePawn->SetPhysicsLinearVelocity(Velocity);
}
#pragma endregion 

