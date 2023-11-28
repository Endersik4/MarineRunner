// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Door/Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DoorWidgets/DoorPanelWidget.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DoorSkeletalMesh"));
	RootComponent = DoorSkeletalMesh;

	DoorPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Panel Mesh"));
	DoorPanelMesh->SetupAttachment(RootComponent);

	DoorPanelWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Door Panel Widget Component"));
	DoorPanelWidgetComponent->SetupAttachment(DoorPanelMesh);
	DoorPanelWidgetComponent->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	SetUpDoorPanel();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::OpenDoor()
{
	if (bDoorOpen == true || OpenDoorAnim == nullptr)
		return;

	DoorSkeletalMesh->PlayAnimation(OpenDoorAnim, false);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenDoorSound, DoorSkeletalMesh->GetSocketLocation(DoorSoundSocketName));

	bDoorOpen = true;

	GetWorldTimerManager().SetTimer(CloseAfterInactivityHandle, this, &ADoor::CloseDoor, CloseDoorAfterInactivityTime, false);
}

void ADoor::CloseDoor()
{
	if (bDoorOpen == false || CloseDoorAnim == nullptr)
		return;

	GetWorldTimerManager().ClearTimer(CloseAfterInactivityHandle);

	DoorSkeletalMesh->PlayAnimation(CloseDoorAnim, false);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CloseDoorSound, DoorSkeletalMesh->GetSocketLocation(DoorSoundSocketName));

	bDoorOpen = false;
}

void ADoor::SetUpDoorPanel()
{
	DoorPanelWidget = Cast<UDoorPanelWidget>(DoorPanelWidgetComponent->GetUserWidgetObject());
	if (IsValid(DoorPanelWidget) == false)
		return;

	DoorPanelWidget->SetDoorActor(this);

	if (bUsePinCode == true)
		DoorPanelWidget->ChangeDoorPanelToUsePin();
}

