// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/OutsideElevatorDoor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Elevator.h"
#include "CallElevatorPanel.h"
#include "MarineRunner/Objects/SavedDataObject.h"

// Sets default values
AOutsideElevatorDoor::AOutsideElevatorDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OutsideElevatorDoorsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Outside Elevator Doors Skeletal Mesh"));
	RootComponent = OutsideElevatorDoorsSkeletalMesh;

	OutsideElevatorPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Outside Elevator Panel Mesh"));
	OutsideElevatorPanelMesh->SetupAttachment(RootComponent);

	OutsideElevatorPanel = CreateDefaultSubobject<UWidgetComponent>(TEXT("Outside Elevator Panel"));
	OutsideElevatorPanel->SetupAttachment(RootComponent);
	OutsideElevatorPanel->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void AOutsideElevatorDoor::BeginPlay()
{
	Super::BeginPlay();
	
	SetUpElevatorPanel();
}

// Called every frame
void AOutsideElevatorDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/// <summary>
/// Opens the door if can
/// </summary>
void AOutsideElevatorDoor::OpenOutsideElevatorDoor()
{
	if (bDoorsOpen == true)
		return;

	PlayElevatorEffects(OpenElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorsOpen = true;
}

/// <summary>
/// Closes the door if can
/// </summary>
void AOutsideElevatorDoor::CloseOutsideElevatorDoor()
{
	if (bDoorsOpen == false)
		return;

	PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorsOpen = false;
}

void AOutsideElevatorDoor::PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay)
{
	if (AnimToPlay)
		OutsideElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);

	if (SoundToPlay)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, OutsideElevatorDoorsSkeletalMesh->GetSocketLocation(SoundLocationSocketName));
}

void AOutsideElevatorDoor::SetUpElevatorPanel()
{
	ElevatorPanelWidget = Cast<UCallElevatorPanel>(OutsideElevatorPanel->GetUserWidgetObject());
	if (ElevatorPanelWidget == nullptr) return;

	ElevatorPanelWidget->SetFloor(Floor);
	ElevatorPanelWidget->SetElevator(ElevatorToCall);
	ElevatorPanelWidget->SetOutsideElevatorDoor(this);

	if (bUsePinCode == true)
		ElevatorPanelWidget->ChangeDoorPanelToUsePin(PinCode);
}

bool AOutsideElevatorDoor::CanCallElevator() const
{
	return ElevatorPanelWidget->CanCallElevator();
}

void AOutsideElevatorDoor::CallElevatorAction(ECallElevatorAction ActionToDo)
{
	ElevatorPanelWidget->CallElevatorAction(ActionToDo);
}

void AOutsideElevatorDoor::PinIsCorrect()
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void AOutsideElevatorDoor::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1 && IsValid(ElevatorPanelWidget))
	{
		ElevatorPanelWidget->PinIsCorrect();
	}
}

void AOutsideElevatorDoor::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}
