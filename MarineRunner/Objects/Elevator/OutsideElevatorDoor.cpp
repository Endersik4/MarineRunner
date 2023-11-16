// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/OutsideElevatorDoor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Elevator.h"
#include "CallElevatorPanel.h"

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
	
	ElevatorPanelWidget = Cast<UCallElevatorPanel>(OutsideElevatorPanel->GetUserWidgetObject());
	if (ElevatorPanelWidget == nullptr) return;

	ElevatorPanelWidget->SetElevator(ElevatorToCall);
	ElevatorPanelWidget->SetFloor(Floor);
}

// Called every frame
void AOutsideElevatorDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOutsideElevatorDoor::OpenOutsideElevatorDoor()
{
	if (bDoorsOpen == true)
		return;

	PlayElevatorEffects(OpenElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorsOpen = true;
}

void AOutsideElevatorDoor::CloseOutsideElevatorDoor()
{
	if (bDoorsOpen == false)
		return;

	PlayElevatorEffects(CloseElevatorDoorsAnim, OpenElevatorDoorsSound);
	bDoorsOpen = false;
}

void AOutsideElevatorDoor::ActiveCallElevatorPanel()
{
	ElevatorPanelWidget->ActiveCallElevatorPanel();
}

void AOutsideElevatorDoor::PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay)
{
	if (AnimToPlay)
		OutsideElevatorDoorsSkeletalMesh->PlayAnimation(AnimToPlay, false);

	if (SoundToPlay)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToPlay, OutsideElevatorDoorsSkeletalMesh->GetSocketLocation(SoundLocationSocketName));
}

