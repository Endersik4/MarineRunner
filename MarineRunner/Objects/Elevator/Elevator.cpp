// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include "ElevatorPanelWidget.h"
#include "MarineRunner/Objects/Elevator/OutsideElevatorDoor.h"
#include "MarineRunner/Objects/SavedDataObject.h"

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

	Tags.Add(FName(TEXT("Elevator")));
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	SetUpElevatorPanel();
}

void AElevator::SetUpElevatorPanel()
{
	ElevatorPanelWidget = Cast<UElevatorPanelWidget>(ElevatorPanel->GetUserWidgetObject());
	if (IsValid(ElevatorPanelWidget) == false)
		return;

	ElevatorPanelWidget->SetElevator(this);
	ElevatorPanelWidget->SetElevatorFloors(ElevatorFloors);

	ElevatorPanelWidget->FillSelectFloorsListView();
}

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElevatorIsMoving(DeltaTime);
}

void AElevator::PrepareElevatorToMove(FVector Location, int32 Floor)
{
	StartLocation = GetActorLocation();
	FloorLocationToGo = Location;

	GetWorldTimerManager().ClearTimer(CloseDoorAfterInactivityHandle);

	// if the elevator is on the same floor as the floor the player wants to go to then open/close the door
	if (CurrentFloor == Floor)
	{
		MoveTimeElapsed += TimeToMoveOnFloor;
		CurrentOutsideElevatorDoor = nullptr;
		MovedToNewFoor();

		return;
	}

	// Check if there is outside elevator door, if there is then close outside doors and show Wait Effect on its panel
	CurrentOutsideElevatorDoor = OutsideElevatorDoors.Find(CurrentFloor) ? *OutsideElevatorDoors.Find(CurrentFloor) : nullptr;
	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CloseOutsideElevatorDoor();
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowWaitEffect);
	}

	CurrentFloor = Floor;
	SaveElevatorState(Floor);

	// If the door is open, wait until it closes and start moving the elevator, if it is not open, start moving the elevator.
	if (bDoorOpen == true)
	{
		PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
		ElevatorPanelWidget->ShowWaitForElevatorText(true);
		bDoorOpen = false;

		GetWorld()->GetTimerManager().SetTimer(StartElevatorHandle, this, &AElevator::StartMovingElevator, CloseElevatorDoorsAnim->GetPlayLength(), false);
		return;
	}

	StartMovingElevator();
}

void AElevator::StartMovingElevator()
{
	ElevatorPanelWidget->ShowWaitForElevatorText(false);
	ElevatorPanelWidget->ShowElevatorGoesUpDownImage(true, FloorLocationToGo);

	SpawnedAmbientElevatorSound = UGameplayStatics::SpawnSoundAttached(AmbientElevatorSound, ElevatorMesh);
	SpawnedAmbientElevatorSound->FadeIn(2.f);

	bCanMoveToFloorLocation = true;
}

void AElevator::ElevatorIsMoving(float Delta)
{
	if (bCanMoveToFloorLocation == false)
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
	bCanMoveToFloorLocation = false;
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

	OpenOutsideElevatorDoors();

	ElevatorPanelWidget->ShowElevatorGoesUpDownImage(false);
	ElevatorPanelWidget->ShowWaitForElevatorText();

	GetWorld()->GetTimerManager().SetTimer(StartElevatorHandle, this, &AElevator::CanUseElevatorAgain, CanUseElevatorAgainDelay, false);
}

void AElevator::OpenOutsideElevatorDoors()
{
	AOutsideElevatorDoor** DbPtrToOutsideElevatorDoor = OutsideElevatorDoors.Find(CurrentFloor);
	CurrentOutsideElevatorDoor = DbPtrToOutsideElevatorDoor ? *DbPtrToOutsideElevatorDoor : nullptr;
	if (IsValid(CurrentOutsideElevatorDoor) == false)
		return;

	CurrentOutsideElevatorDoor->OpenOutsideElevatorDoor();

	if (CurrentOutsideElevatorDoor->CanCallElevator())
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideCallAndShowWait);
}

void AElevator::CanUseElevatorAgain()
{
	ElevatorPanelWidget->ShowWaitForElevatorText(false);
	ElevatorPanelWidget->ShowSelectFloorPanel(true);

	GetWorldTimerManager().SetTimer(CloseDoorAfterInactivityHandle, this, &AElevator::CloseElevatorDoors, TimeToCloseDoorsAfterInactivity, false);

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

void AElevator::CloseElevatorDoors()
{
	ElevatorPanelWidget->ShowSelectFloorPanel(false);
	ElevatorPanelWidget->ShowWaitForElevatorText();

	PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorOpen = false;

	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CloseOutsideElevatorDoor();
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowWaitEffect);
	}

	FTimerHandle ElevatorDoorHandle;
	GetWorldTimerManager().SetTimer(ElevatorDoorHandle, this, &AElevator::ActivateElevatorDoors, CloseElevatorDoorsAnim->GetPlayLength(), false);
}

void AElevator::ActivateElevatorDoors()
{
	ElevatorPanelWidget->ShowWaitForElevatorText(false);
	ElevatorPanelWidget->ShowSelectFloorPanel(true);

	if (IsValid(CurrentOutsideElevatorDoor) == true)
	{
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideWaitEffect);
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowCall);
	}
}

void AElevator::PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay)
{
	if (AnimToPlay)
		ElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);

	if (SoundToPlay)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, ElevatorDoorsSkeletalMesh->GetSocketLocation(DoorSoundSocketName));
}

void AElevator::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	FElevatorFloor* FoundFloor = ElevatorFloors.FindByKey((int)SavedCustomData.ValueToSave);
	if (FoundFloor == nullptr)
		return;

	CurrentUniqueID = IDkey;
	CurrentFloor = (int)SavedCustomData.ValueToSave;
	SetActorLocation(FoundFloor->FloorLocation);
}

void AElevator::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AElevator::SaveElevatorState(int32 SaveState)
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1, SaveState));
}