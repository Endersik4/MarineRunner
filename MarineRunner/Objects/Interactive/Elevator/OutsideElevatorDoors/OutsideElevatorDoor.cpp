// Copyright Adam Bartela.All Rights Reserved


#include "OutsideElevatorDoor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Objects/Interactive/Elevator/Elevator.h"
#include "CallElevatorPanelWidget.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

// Sets default values
AOutsideElevatorDoor::AOutsideElevatorDoor()
{
	PrimaryActorTick.bCanEverTick = false;

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

void AOutsideElevatorDoor::OpenOutsideElevatorDoor()
{
	if (bDoorsOpen)
		return;

	PlayElevatorEffects(OpenElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorsOpen = true;
}

void AOutsideElevatorDoor::CloseOutsideElevatorDoor()
{
	if (!bDoorsOpen)
		return;

	PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorsOpen = false;
}

void AOutsideElevatorDoor::PlayElevatorEffects(TObjectPtr<UAnimationAsset> AnimToPlay, TObjectPtr<USoundBase> SoundToPlay)
{
	if (IsValid(AnimToPlay))
		OutsideElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);

	if (IsValid(SoundToPlay))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, OutsideElevatorDoorsSkeletalMesh->GetSocketLocation(SoundLocationSocketName));
}

void AOutsideElevatorDoor::SetUpElevatorPanel()
{
	ElevatorPanelWidget = Cast<UCallElevatorPanel>(OutsideElevatorPanel->GetUserWidgetObject());
	if (!ElevatorPanelWidget) 
		return;

	ElevatorPanelWidget->SetFloor(FloorNumber);
	ElevatorPanelWidget->SetElevator(ElevatorToCall);
	ElevatorPanelWidget->SetOutsideElevatorDoor(this);

	if (bUsePinCode)
		ElevatorPanelWidget->ChangeToUsePin(PinCode);
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
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
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

void AOutsideElevatorDoor::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (SavedCustomData.ObjectState == 1)
	{
		if (bUsePinCode == true)
			ElevatorPanelWidget->ChangeToUsePin(PinCode);
	}
}
