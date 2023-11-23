// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include "ElevatorPanelWidget.h"
#include "MarineRunner/Objects/Elevator/OutsideElevatorDoor.h"

// Sets default values
AElevator::AElevator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ElevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator Mesh"));
	RootComponent = ElevatorMesh;

	ElevatorDoorsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Elevator Doors Skeletal Mesh"));
	ElevatorDoorsSkeletalMesh->SetupAttachment(RootComponent);

	ElevatorPanel = CreateDefaultSubobject<UWidgetComponent>(TEXT("Elevator Panel"));
	ElevatorPanel->SetupAttachment(RootComponent);
	ElevatorPanel->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	ElevatorPanelWidget = Cast<UElevatorPanelWidget>(ElevatorPanel->GetUserWidgetObject());
	if (ElevatorPanelWidget == nullptr) return;

	ElevatorPanelWidget->SetElevator(this);
	ElevatorPanelWidget->FillSelectFloorsListView();
}

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToFloor(DeltaTime);
}

void AElevator::StartElevator(FVector Location, int32 Floor)
{
	StartLocation = GetActorLocation();
	FloorLocationToGo = Location;

	if (CurrentFloor == Floor)
	{
		MoveTimeElapsed += TimeToMoveOnFloor;
		CurrentOutsideElevatorDoor = nullptr;
		UE_LOG(LogTemp, Error, TEXT("=="));

		MovedToNewFoor();
		return;
	}

	CurrentOutsideElevatorDoor = OutsideElevatorDoors.Find(CurrentFloor) ? *OutsideElevatorDoors.Find(CurrentFloor) : nullptr;
	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CloseOutsideElevatorDoor();
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowWaitEffect);
	}

	// If the door is open, wait until it closes and start moving the elevator, if it is not open, start moving the elevator.
	if (bDoorOpen == true)
	{
		PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
		bDoorOpen = false;
		UE_LOG(LogTemp, Warning, TEXT("TIME %f"), CloseElevatorDoorsAnim->GetMaxCurrentTime());
		//CloseElevatorDoorsAnim->GetMaxCurrentTime();
		CurrentFloor = Floor;

		GetWorld()->GetTimerManager().SetTimer(StartElevatorHandle, this, &AElevator::StartMovingElevator, StartElevatorDelay, false);
		return;
	}

	CurrentFloor = Floor;
	StartMovingElevator();
}

void AElevator::StartMovingElevator()
{
	ElevatorPanelWidget->ActivateElevatorGoesUpDownImage(true, FloorLocationToGo);
	SpawnedAmbientElevatorSound = UGameplayStatics::SpawnSoundAttached(AmbientElevatorSound, ElevatorMesh);
	SpawnedAmbientElevatorSound->FadeIn(2.f);

	bShouldMove = true;
}

void AElevator::MoveToFloor(float Delta)
{
	if (bShouldMove == false)
		return;

	if (MoveTimeElapsed <= TimeToMoveOnFloor)
	{
		FVector NewLocation = FMath::Lerp(StartLocation, FloorLocationToGo, MoveTimeElapsed / TimeToMoveOnFloor);
		SetActorLocation(NewLocation);

		MoveTimeElapsed += Delta;
	}
	else
	{
		MovedToNewFoor();
	}
}

void AElevator::MovedToNewFoor()
{
	SetActorLocation(FloorLocationToGo);
	bShouldMove = false;
	MoveTimeElapsed = 0.f;

	if (IsValid(SpawnedAmbientElevatorSound))
	{
		SpawnedAmbientElevatorSound->FadeOut(2.f, 0.f);
	}

	if (IsValid(CurrentOutsideElevatorDoor)) 
		BeforeCurrentOutsideElevatorDoor = CurrentOutsideElevatorDoor;

	if (bDoorOpen == false)
	{
		PlayElevatorEffects(OpenElevatorDoorsAnim, OpenElevatorDoorsSound);
		bDoorOpen = true;
	}

	AOutsideElevatorDoor** DbPtrToOutsideElevatorDoor = OutsideElevatorDoors.Find(CurrentFloor);
	CurrentOutsideElevatorDoor = DbPtrToOutsideElevatorDoor ? *DbPtrToOutsideElevatorDoor : nullptr;
	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->OpenOutsideElevatorDoor();

		if (CurrentOutsideElevatorDoor->CanCallElevator()) 
			CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideCallAndShowWait);
	}

	ElevatorPanelWidget->ActivateElevatorGoesUpDownImage(false);
	ElevatorPanelWidget->ActivateWaitForElevatorText();

	GetWorld()->GetTimerManager().SetTimer(StartElevatorHandle, this, &AElevator::CanUseElevatorAgain, CanUseElevatorAgainDelay, false);
}

void AElevator::CanUseElevatorAgain()
{
	ElevatorPanelWidget->ActiveSelectFloorPanel(true);

	FTimerHandle CloseElevatorDoorHandle;
	GetWorldTimerManager().SetTimer(CloseElevatorDoorHandle, this, &AElevator::CloseElevatorDoors, TimeToCloseDoorsAfterInactivity, false);

	if (IsValid(BeforeCurrentOutsideElevatorDoor))
	{
		BeforeCurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideWaitEffect);
		BeforeCurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowCall);
	}

	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideWaitEffect);
	}
}

void AElevator::PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay)
{
	if (AnimToPlay)
		ElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);

	if (SoundToPlay) 
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, ElevatorDoorsSkeletalMesh->GetSocketLocation(SoundLocationSocketName));
}

void AElevator::CloseElevatorDoors()
{
	ElevatorPanelWidget->ActiveSelectFloorPanel(false);
	ElevatorPanelWidget->ActivateWaitForElevatorText();

	PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorOpen = false;

	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CloseOutsideElevatorDoor();
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowWaitEffect);
	}

	FTimerHandle ElevatorDoorHandle;
	GetWorldTimerManager().SetTimer(ElevatorDoorHandle, this, &AElevator::ActivateElevatorDoors, StartElevatorDelay, false);
}

void AElevator::ActivateElevatorDoors()
{
	ElevatorPanelWidget->ActiveSelectFloorPanel(true);

	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideWaitEffect);
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowCall);
	}
}
