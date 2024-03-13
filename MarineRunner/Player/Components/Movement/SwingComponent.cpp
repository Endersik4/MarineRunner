// Copyright Adam Bartela.All Rights Reserved


#include "SwingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/Movement/CroachAndSlideComponent.h"
#include "MarineRunner/Objects/Interactive/Hook.h"
#include "MarineRunner/Objects/Effects/SwingLine.h"

// Sets default values for this component's properties
USwingComponent::USwingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USwingComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMarineCharacter>(GetOwner());
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

// Called every frame
void USwingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	MovingToHook(DeltaTime);
	HookLineCheck();
}

#pragma region ///////////////////////////////// SWING /////////////////////////////////
void USwingComponent::HookLineCheck()
{
	if (!bCanMakeSwingLineCheck || bIsPlayerMovingToHook) 
		return;

	FVector HookCheckLineStart = Player->GetCamera()->GetComponentLocation();
	FVector HookCheckLineEnd = HookCheckLineStart + (PlayerController->GetRootComponent()->GetForwardVector() * LengthOfSwingLineRaycast);

	FHitResult HitResults;
	bool bHookHovered = GetWorld()->LineTraceSingleByChannel(HitResults, HookCheckLineStart, HookCheckLineEnd, ECC_GameTraceChannel2);

	if (bHookHovered)
		ActivateCurrentHoveredHook(HitResults.GetActor());
	else 
		ClearLastActivatedHook();
}

void USwingComponent::ActivateCurrentHoveredHook(TObjectPtr<AActor> HookActorFromHit)
{
	TObjectPtr<AHook> HoveredHook = Cast<AHook>(HookActorFromHit);
	if (!IsValid(HoveredHook))
		return;

	if (HoveredHook == CurrentFocusedHook)
		return;

	// Disable previous hook if there was any
	if (IsValid(CurrentFocusedHook))
		CurrentFocusedHook->ChangeToIdleAnim();

	if (!HoveredHook->GetCanGrabHook())
		return;

	bCanPlayerSwing = true;
	HoveredHook->ChangeToPlayerInRangeAnim();
	CurrentFocusedHook = HoveredHook;
}

void USwingComponent::ClearLastActivatedHook()
{
	if (!IsValid(CurrentFocusedHook)|| bWasSwingPressed)
		return;

	CurrentFocusedHook->ChangeToIdleAnim();
	CurrentFocusedHook = nullptr;

	bCanPlayerSwing = false;
}

//If the player press the Swing button then spawn the Line that is going to the Hook and then wait for SwingDelay to elapsed
void USwingComponent::SwingPressed()
{
	if (!IsValid(CurrentFocusedHook))
		return;

	if (!bCanPlayerSwing)
		return;

	if (bIsPlayerMovingToHook || bWasSwingPressed)
		return;

	CurrentFocusedHook->StartHookCooldown();
	bWasSwingPressed = true;
	bCanMakeSwingLineCheck = false;

	SpawnSwingEffects();

	GetWorld()->GetTimerManager().SetTimer(StartSwingHandle, this, &USwingComponent::PrepareMoveToHook, SwingDelay);
}

void USwingComponent::SpawnSwingEffects()
{
	if (IsValid(SwingSound)) 
		UGameplayStatics::SpawnSound2D(GetWorld(), SwingSound);

	FVector SpawnLocation = Player->GetCamera()->GetComponentLocation();
	SpawnLocation += PlayerController->GetRootComponent()->GetForwardVector() * ForwardSwingLineLocationOffset + PlayerController->GetRootComponent()->GetRightVector() * RightSwingLineLocationOffset;
	
	TObjectPtr<ASwingLine> SwingLine = GetWorld()->SpawnActor<ASwingLine>(SwingLineClass, SpawnLocation, FRotator(0, 0, 0));
	if (!IsValid(SwingLine))
		return;

	SwingLine->StartMovingToHookLocation(CurrentFocusedHook->GetActorLocation() - CurrentFocusedHook->GetSwingLineLocationOffset(), SwingDelay);
}

void USwingComponent::PrepareMoveToHook()
{
	if (!IsValid(CurrentFocusedHook)) 
		return;

	Player->GetPlayerCapsule()->SetPhysicsLinearVelocity(FVector(0));
	bIsPlayerMovingToHook = true;

	HookLocation = CurrentFocusedHook->GetActorLocation() - CurrentFocusedHook->GetHookLocationOffset();

	FVector DirectionTowardsHook = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation(), HookLocation));
	FVector ImpulseTowardsHook = DirectionTowardsHook * SwingImpulse;
	Player->GetPlayerCapsule()->AddImpulse(ImpulseTowardsHook);

	Player->GetCrouchAndSlideComponent()->CrouchReleased();
}

//Interp player to location of the Hook
void USwingComponent::MovingToHook(float Delta)
{
	if (!bIsPlayerMovingToHook) 
		return;

	FVector NewLocation = FMath::VInterpTo(Player->GetActorLocation(), HookLocation, Delta, SwingSpeed);
	Player->SetActorLocation(NewLocation);

	StopMovingToHook();
}
void USwingComponent::StopMovingToHook()
{
	if (!Player->GetCamera()->GetComponentLocation().Equals(HookLocation, MaxHookDistanceToFinishMoving))
		return;

	bWasSwingPressed = false;
	bIsPlayerMovingToHook = false;
	bCanMakeSwingLineCheck = true;
	CurrentFocusedHook = nullptr;

	FVector NewVelocity = Player->GetPlayerCapsule()->GetPhysicsLinearVelocity() * (SwingLinearPhysicsMultiplier / UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
	NewVelocity.Z = Player->GetPlayerCapsule()->GetPhysicsLinearVelocity().Z;
	Player->GetPlayerCapsule()->SetPhysicsLinearVelocity(NewVelocity);
}
#pragma endregion 

