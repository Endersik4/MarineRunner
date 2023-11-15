// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
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

	//PlayElevatorEffects(OpenElevatorDoorsAnim, OpenElevatorDoorsSound);
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
	FloorToGo = Floor;

	if (bDoorOpen == true)
	{
		PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
		bDoorOpen = false;

		AOutsideElevatorDoor* CurrentOutsideElevatorDoor = *OutsideElevatorDoors.Find(FloorToGo);
		if (IsValid(CurrentOutsideElevatorDoor) == true)
		{
			CurrentOutsideElevatorDoor->CloseOutsideElevatorDoor();
		}
	}

	GetWorld()->GetTimerManager().SetTimer(StartElevatorHandle, this, &AElevator::StartMovingElevator, StartElevatorDelay, false);
}

void AElevator::StartMovingElevator()
{
	ElevatorPanelWidget->ActivateElevatorGoesUpDownImage(true, FloorLocationToGo);
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

	if (bDoorOpen == false)
	{
		PlayElevatorEffects(OpenElevatorDoorsAnim, OpenElevatorDoorsSound);
		bDoorOpen = true;

		AOutsideElevatorDoor * CurrentOutsideElevatorDoor = *OutsideElevatorDoors.Find(FloorToGo);
		if (IsValid(CurrentOutsideElevatorDoor) == true)
		{
			CurrentOutsideElevatorDoor->OpenOutsideElevatorDoor();
		}
	}

	ElevatorPanelWidget->ActivateElevatorGoesUpDownImage(false);
	ElevatorPanelWidget->ActivateWaitForElevatorText();

	FTimerDelegate CanSelectFloorTimerDel;
	CanSelectFloorTimerDel.BindUFunction(ElevatorPanelWidget, FName("ActiveSelectFloorPanel"), true);
	GetWorld()->GetTimerManager().SetTimer(StartElevatorHandle, CanSelectFloorTimerDel, CanUseElevatorAgainDelay, false);

}

void AElevator::PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay)
{
	if (AnimToPlay)
		ElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);

	if (SoundToPlay) 
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, ElevatorDoorsSkeletalMesh->GetSocketLocation(SoundLocationSocketName));
}

