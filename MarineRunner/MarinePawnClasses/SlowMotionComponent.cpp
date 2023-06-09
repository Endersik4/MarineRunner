// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowMotionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"

// Sets default values for this component's properties
USlowMotionComponent::USlowMotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USlowMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (MarinePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SLOW MOTIOn COMPONENT!"));
}


// Called every frame
void USlowMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AddingVelocity();
}

void USlowMotionComponent::SlowMotionPressed()
{
	if (bIsInSlowMotion)
	{
		if (SlowMotionSoundSpawned) SlowMotionSoundSpawned->Stop();
		if (CancelSlowMotionSound) UGameplayStatics::PlaySound2D(GetWorld(), CancelSlowMotionSound);
		MarinePawn->RemoveDashWidget();

		GetWorld()->GetTimerManager().ClearTimer(SlowMotionTimeHandle);
		DisableSlowMotion();
		
	}

	if (bCanSlowMotion == false) return;

	SettingUpSlowMotion();

	GetWorld()->GetTimerManager().SetTimer(SlowMotionTimeHandle, this, &USlowMotionComponent::DisableSlowMotion, SlowMotionValue * SlowMotionTime);
}

void USlowMotionComponent::SettingUpSlowMotion()
{
	bCanSlowMotion = false;
	bIsInSlowMotion = true;
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), 0.5f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	//If in Marine pawn there is adding a counter movement then disable this and add impulse in direction of what player pressed (forward,right...)
	//When this happens then Player in Air move like Physics object 
	if (MarinePawn->GetShouldAddCounterMovement())
	{
		MarinePawn->SetShouldAddCounterMovement(false);

		FVector Impulse;
		if (MarinePawn->GetInputAxisValue("Forward") > 0) Impulse += MarinePawn->GetActorForwardVector();
		if (MarinePawn->GetInputAxisValue("Forward") < 0) Impulse += -MarinePawn->GetActorForwardVector();
		if (MarinePawn->GetInputAxisValue("Right") > 0) Impulse += -MarinePawn->GetActorRightVector();
		if (MarinePawn->GetInputAxisValue("Right") < 0) Impulse += MarinePawn->GetActorRightVector();
		//If player dont press keys related to movement then do not add impulse 
		if (!(Impulse == FVector(0, 0, 0)))
		{
			Impulse *= 900.f * 1000.f;
			Impulse = FRotator(0, Impulse.Rotation().Yaw, 0).Vector();
		}
		MarinePawn->SetMovementImpulse(Impulse);
	}
	if (SlowMotionSound) SlowMotionSoundSpawned = UGameplayStatics::SpawnSound2D(GetWorld(), SlowMotionSound);

	//SlowMotion command
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowMotionValue);
	MarinePawn->CustomTimeDilation = 2.5f;

	//DashWidget will be on player viewport for some time (SlowMotionTime)
	MarinePawn->MakeDashWidget(true, SlowMotionTime-1.0f, false); 
	ElementBar SlowMoElementBar{ SlowMotionDelay + 1.7f}, ButtonSlowMoElementBar{ 0.3f };
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::SlowMo, SlowMoElementBar);
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::Button_SlowMo, ButtonSlowMoElementBar);
}

void USlowMotionComponent::DisableSlowMotion()
{
	bIsInSlowMotion = false;
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), 1.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	MarinePawn->CustomTimeDilation = 1.f;

	//Enable delay for SlowMotion
	GetWorld()->GetTimerManager().SetTimer(SlowMotionDelayHandle, this, &USlowMotionComponent::DelayCompleted, SlowMotionDelay);
}

void USlowMotionComponent::DelayCompleted()
{
	bCanSlowMotion = true; 
}

void USlowMotionComponent::AddingVelocity()
{
	if (bIsInSlowMotion == false) return;

	FVector Vel = MarinePawn->CapsulePawn->GetPhysicsLinearVelocity();
	Vel /= 1.1f;
	MarinePawn->CapsulePawn->SetPhysicsLinearVelocity(Vel);
}

