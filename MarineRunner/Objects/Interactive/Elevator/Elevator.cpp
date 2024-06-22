// Copyright Adam Bartela.All Rights Reserved


#include "Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ListView.h"
#include "Kismet/KismetSystemLibrary.h"

#include "ElevatorPanel/ElevatorPanelWidget.h"
#include "OutsideElevatorDoors/OutsideElevatorDoor.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

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

	Tags.Add(FName(TEXT("Elevator"))); // When player is in elevator then he cant save ggame
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	SetUpElevatorPanel();

	if (bMoveElevatorAfterTime)
		MoveElevatorAfterTime();
}

void AElevator::SetUpElevatorPanel()
{
	if (!IsValid(ElevatorPanel->GetUserWidgetObject()))
		return;

	ElevatorPanelWidget = Cast<UElevatorPanelWidget>(ElevatorPanel->GetUserWidgetObject());
	if (!IsValid(ElevatorPanelWidget))
		return;

	ElevatorPanelWidget->SetElevator(this);
	ElevatorPanelWidget->SetElevatorFloors(ElevatorFloors);

	ElevatorPanelWidget->FillSelectFloorsListView();
}

#pragma region //////////// CUTSCENE /////////////
void AElevator::MoveElevatorAfterTime()
{
	if (!IsPlayerTooFarAwayToDoCutscene())
		return;

	ElevatorPanelWidget->SelectFloorsListView->SetVisibility(ESlateVisibility::Hidden);

	FTimerHandle MoveToFloorHandle;
	FTimerDelegate MoveToFloorDelegate = FTimerDelegate::CreateUObject(this, &AElevator::PrepareElevatorToMove, FloorToMoveWhileCutscene.FloorLocation, FloorToMoveWhileCutscene.Floor);
	GetWorldTimerManager().SetTimer(MoveToFloorHandle, MoveToFloorDelegate, MoveToFloorTimeInCutscene, false);
}

bool AElevator::IsPlayerTooFarAwayToDoCutscene()
{
	TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(Player))
		return true;

	if (FVector::Dist(GetActorLocation(), Player->GetActorLocation()) > MaxDistanceToPlayerToDoCutscene)
		return false;

	return true;
}
#pragma endregion

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElevator::PrepareElevatorToMove(FVector Location, int32 Floor)
{
	FloorLocationToGo = Location;

	GetWorldTimerManager().ClearTimer(CloseDoorAfterInactivityHandle);

	// if the elevator is on the same floor as the floor the player wants to go to then open/close the door
	if (CurrentFloor == Floor)
	{
		CurrentOutsideElevatorDoor = nullptr;
		MovedToNewFloor();

		return;
	}

	// Check if there is outside elevator door, if there is then close outside doors and show Wait Effect on its panel
	CurrentOutsideElevatorDoor = OutsideElevatorDoors.Find(CurrentFloor) ? *OutsideElevatorDoors.Find(CurrentFloor) : nullptr;
	if (IsValid(CurrentOutsideElevatorDoor))
	{
		CurrentOutsideElevatorDoor->CloseOutsideElevatorDoor();
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowWaitEffect);
	}

	CurrentFloor = Floor;
	SaveElevatorState(Floor);

	// If the door is open, wait until it closes and start moving the elevator, if it is not open, start moving the elevator.
	if (bDoorOpen)
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
	ElevatorMesh->RecreatePhysicsState();

	if (IsValid(AmbientElevatorSound))
	{
		SpawnedAmbientElevatorSound = UGameplayStatics::SpawnSoundAttached(AmbientElevatorSound, ElevatorMesh);

		if (IsValid(SpawnedAmbientElevatorSound))
			SpawnedAmbientElevatorSound->FadeIn(2.f);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Spawned Ambient Elevator Sound is nullptr in Elevator!"));

	FLatentActionInfo MoveElevatorToFloorLatentAction;
	MoveElevatorToFloorLatentAction.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(ElevatorMesh, FloorLocationToGo, GetActorRotation(), true, true, TimeToMoveOnFloor, false, EMoveComponentAction::Move, MoveElevatorToFloorLatentAction);

	GetWorld()->GetTimerManager().SetTimer(NewFloorHandle, this, &AElevator::MovedToNewFloor, TimeToMoveOnFloor, false);
}

void AElevator::MovedToNewFloor()
{
	SetActorLocation(FloorLocationToGo);

	if (IsValid(SpawnedAmbientElevatorSound))
	{
		SpawnedAmbientElevatorSound->FadeOut(2.f, 0.f);
	}

	if (IsValid(CurrentOutsideElevatorDoor)) 
		BeforeCurrentOutsideElevatorDoor = CurrentOutsideElevatorDoor;

	if (!bDoorOpen)
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
	if (!OutsideElevatorDoors.Find(CurrentFloor))
		return;

	CurrentOutsideElevatorDoor = *OutsideElevatorDoors.Find(CurrentFloor);
	if (!IsValid(CurrentOutsideElevatorDoor))
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

	if (IsValid(CurrentOutsideElevatorDoor))
		CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideWaitEffect);
}

void AElevator::CloseElevatorDoors()
{
	ElevatorPanelWidget->ShowSelectFloorPanel(false);
	ElevatorPanelWidget->ShowWaitForElevatorText();

	PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorOpen = false;

	if (IsValid(CurrentOutsideElevatorDoor))
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

	if (!IsValid(CurrentOutsideElevatorDoor))
		return;

	CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_HideWaitEffect);
	CurrentOutsideElevatorDoor->CallElevatorAction(ECEA_ShowCall);
}

void AElevator::PlayElevatorEffects(TObjectPtr < UAnimationAsset> AnimToPlay, TObjectPtr < USoundBase> SoundToPlay)
{
	if (IsValid(AnimToPlay))
		ElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);
	else 
		UE_LOG(LogTemp, Warning, TEXT("Elevator Animation is nullptr in Elevator!"));

	if (IsValid(SoundToPlay))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, ElevatorDoorsSkeletalMesh->GetSocketLocation(DoorSoundSocketName));
	else
		UE_LOG(LogTemp, Warning, TEXT("Elevator Sound is nullptr in Elevator!"));
}

void AElevator::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	FElevatorFloor* FoundFloor = ElevatorFloors.FindByKey((int)SavedCustomData.ValueToSave);
	if (!FoundFloor)
		return;

	CurrentUniqueID = IDkey;
	CurrentFloor = (int)SavedCustomData.ValueToSave;
	SetActorLocation(FoundFloor->FloorLocation);
}

void AElevator::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AElevator::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AElevator::SaveElevatorState(int32 SaveState)
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1, SaveState));
}