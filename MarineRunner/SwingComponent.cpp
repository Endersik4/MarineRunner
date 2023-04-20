// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineCharacter.h"
#include "Hook.h"
#include "SwingLine.h"

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

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (MarinePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SWING COMPONENT!"));
	
}


// Called every frame
void USwingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SwingInterp(DeltaTime);
	SwingLineCheck();
}

void USwingComponent::SwingPressed()
{
	if (!bCanMarineSwing || HookCast == nullptr || bCanSwingLerp == true) return;

	FVector SpawnLocation = MarinePawn->GetCamera()->GetComponentLocation();
	SpawnLocation += MarinePawn->GetCamera()->GetForwardVector() * 100.f;
	SpawnLocation += MarinePawn->GetCamera()->GetRightVector() * 70.f;
	ASwingLine* SwingLine = GetWorld()->SpawnActor<ASwingLine>(SwingLineClass, SpawnLocation, FRotator(0, 0, 0));
	if (SwingLine)
	{
		SwingLine->SetHookLocation(HookCast->GetActorLocation());
		SwingLine->SetSpeedLine(SwingDelay);
		SwingLine->SetCanTick(true);
	}

	GetWorld()->GetTimerManager().SetTimer(SwingHandle, this, &USwingComponent::Swing, SwingDelay);
	
}

void USwingComponent::Swing()
{
	if (HookCast == nullptr) return;

	MarinePawn->CapsulePawn->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	bCanSwingLerp = true;

	HookLocation = HookCast->GetActorLocation();
	HookLocation.Z -= 50.f;

	FVector DirectionOfVector = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(MarinePawn->GetActorLocation(), HookLocation));
	SwingImpulse = DirectionOfVector * SwingForce * 1000.f;
	MarinePawn->CapsulePawn->AddImpulse(SwingImpulse);

	//Get Direction To Hook but only in X and Z Axis. Its for Pawn Physics
	SwingImpulse = FRotator(0, SwingImpulse.Rotation().Yaw, 0).Vector();
	MarinePawn->SetMovementImpulse(SwingImpulse);
	MarinePawn->SetShouldAddCounterMovement(false);

	//Things that cannot happen while Swing
	MarinePawn->MovementStuffThatCannotHappen();
}

void USwingComponent::SwingInterp(float Delta)
{
	if (!bCanSwingLerp || HookCast == nullptr) return;

	FVector LocationInterp = FMath::VInterpTo(MarinePawn->GetActorLocation(), HookLocation, Delta, SwingSpeed);
	if (MarinePawn->GetCamera()->GetComponentLocation().Equals(HookLocation, 200))
	{
		bCanSwingLerp = false;
		FVector Velocity = MarinePawn->CapsulePawn->GetPhysicsLinearVelocity() * SwingLinearPhysicsMultiplier;
		Velocity.Z = MarinePawn->CapsulePawn->GetPhysicsLinearVelocity().Z;
		MarinePawn->CapsulePawn->SetPhysicsLinearVelocity(Velocity);
	}
	MarinePawn->SetActorLocation(LocationInterp);

}

void USwingComponent::SwingLineCheck()
{
	if (bCanSwingLerp) return;
	FVector LineStart = MarinePawn->GetCamera()->GetComponentLocation();
	FVector LineEnd = LineStart + (MarinePawn->GetCamera()->GetForwardVector() * 2000.f);
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResults;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineStart, LineEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		AHook* TempHook;
		if (HitResults.GetActor()->ActorHasTag("Hook"))
		{
			TempHook = Cast<AHook>(HitResults.GetActor());

			FRotator FoundRotation = UKismetMathLibrary::FindLookAtRotation(MarinePawn->GetCamera()->GetComponentLocation(), TempHook->GetActorLocation());
			if (FoundRotation.Equals(MarinePawn->GetControlRotation(), 60.f))
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
