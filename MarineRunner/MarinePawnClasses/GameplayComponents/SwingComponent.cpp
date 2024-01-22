// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SwingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/Hook.h"
#include "MarineRunner/MarinePawnClasses/SwingLine.h"

// Sets default values for this component's properties
USwingComponent::USwingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

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
	
	SwingInterp(DeltaTime);
	HookLineCheck();
}

#pragma region ///////////////////////////////// SWING /////////////////////////////////
void USwingComponent::HookLineCheck()
{
	if (bCanMakeSwingLineCheck == false || bIsPlayerLerpingToHookPosition == true) 
		return;

	FVector LineStart = MarinePlayer->GetCamera()->GetComponentLocation();
	FVector LineEnd = LineStart + (PlayerController->GetRootComponent()->GetForwardVector() * LengthOfSwingLineRaycast);

	FHitResult HitResults;
	bool bHookHovered = GetWorld()->LineTraceSingleByChannel(HitResults, LineStart, LineEnd, ECC_GameTraceChannel2) && IsValid(HitResults.GetActor());
	if (bHookHovered && HitResults.GetActor()->ActorHasTag("Hook") == true)
	{
		ActivateCurrentHoveredHook(HitResults.GetActor());
	}	
	else ClearLastActivatedHook();
}

void USwingComponent::ActivateCurrentHoveredHook(AActor* HookActorFromHit)
{
	AHook* HoveredHook = Cast<AHook>(HookActorFromHit);

	if (IsValid(HoveredHook) == false)
		return;

	if (HoveredHook == CurrentFocusedHook)
		return;

	// Disable previous hook if there was any
	if (IsValid(CurrentFocusedHook))
		CurrentFocusedHook->ChangeToIdleAnim();

	if (HoveredHook->bCanGrabTheHook == false)
		return;

	bCanPlayerSwing = true;
	HoveredHook->ChangeToPlayerInRangeAnim();
	CurrentFocusedHook = HoveredHook;
}

void USwingComponent::ClearLastActivatedHook()
{
	if (IsValid(CurrentFocusedHook) == false || bWasSwingPressed == true)
		return;

	CurrentFocusedHook->ChangeToIdleAnim();
	CurrentFocusedHook = nullptr;

	bCanPlayerSwing = false;
}

//If the player press the Swing button then spawn the Line that is going to the Hook and then wait for SwingDelay to elapsed
void USwingComponent::SwingPressed()
{
	if (IsValid(CurrentFocusedHook) == false)
		return;

	if (bCanPlayerSwing == false)
		return;

	if (bIsPlayerLerpingToHookPosition == true || bWasSwingPressed == true)
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

	SwingLine->SetHookLocation(CurrentFocusedHook->GetActorLocation()- CurrentFocusedHook->GetSwingLineLocationOffset());
	SwingLine->SetSpeedLine(SwingDelay);
	SwingLine->SetCanTick(true);
}

void USwingComponent::StartSwingToHook()
{
	if (IsValid(CurrentFocusedHook) == false) 
		return;

	MarinePlayer->GetPlayerCapsule()->SetPhysicsLinearVelocity(FVector(0));
	bIsPlayerLerpingToHookPosition = true;

	HookLocation = CurrentFocusedHook->GetActorLocation() - CurrentFocusedHook->GetHookLocationOffset();

	FVector DirectionTowardsHook = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(MarinePlayer->GetActorLocation(), HookLocation));
	SwingImpulse = DirectionTowardsHook * SwingForce;
	MarinePlayer->GetPlayerCapsule()->AddImpulse(SwingImpulse);

	//Things that cannot happen while Swing
	MarinePlayer->MovementStuffThatCannotHappen();
}

//Interp player to location of the Hook
void USwingComponent::SwingInterp(float Delta)
{
	if (!bIsPlayerLerpingToHookPosition) 
		return;

	FVector LocationInterp = FMath::VInterpTo(MarinePlayer->GetActorLocation(), HookLocation, Delta, SwingSpeed);

	StopSwingInterp();

	MarinePlayer->SetActorLocation(LocationInterp);
}
void USwingComponent::StopSwingInterp()
{
	if (MarinePlayer->GetCamera()->GetComponentLocation().Equals(HookLocation, MaxHookDistanceToFinishInterp) == false)
		return;

	bWasSwingPressed = false;
	bIsPlayerLerpingToHookPosition = false;
	bCanMakeSwingLineCheck = true;
	CurrentFocusedHook = nullptr;

	float Multiplier = (SwingLinearPhysicsMultiplier / UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
	FVector Velocity = MarinePlayer->GetPlayerCapsule()->GetPhysicsLinearVelocity() * Multiplier;
	Velocity.Z = MarinePlayer->GetPlayerCapsule()->GetPhysicsLinearVelocity().Z;
	MarinePlayer->GetPlayerCapsule()->SetPhysicsLinearVelocity(Velocity);
}
#pragma endregion 

