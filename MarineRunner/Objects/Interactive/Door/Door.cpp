// Copyright Adam Bartela.All Rights Reserved


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Objects/Interactive/PanelWithPin/PanelWithPinWidget.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DoorSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DoorSkeletalMesh"));
	RootComponent = DoorSkeletalMesh;

	DoorPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Panel Mesh"));
	DoorPanelMesh->SetupAttachment(RootComponent);

	DoorPanelWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Door Panel Widget Component"));
	DoorPanelWidgetComponent->SetupAttachment(DoorPanelMesh);
	DoorPanelWidgetComponent->SetDrawAtDesiredSize(true);
	DoorPanelWidgetComponent->SetTickWhenOffscreen(true);

	DoorPanelSecondMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Panel Second Mesh"));
	DoorPanelSecondMesh->SetupAttachment(RootComponent);

	DoorPanelSecondWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Door Panel Second Widget Component"));
	DoorPanelSecondWidgetComponent->SetupAttachment(DoorPanelSecondMesh);
	DoorPanelSecondWidgetComponent->SetDrawAtDesiredSize(true);
	DoorPanelSecondWidgetComponent->SetTickWhenOffscreen(true);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	SetUpDoorPanels();
}

void ADoor::ClickedOpenButton(class UDoorPanelWidget* ClickedWidget)
{
	if (!bDoorOpen)
		OpenDoor();
	else
		CloseDoor();

	if (IsValid(GetOtherPanelWidget(ClickedWidget)))
		GetOtherPanelWidget(ClickedWidget)->PlayOpenCloseEffects();
}

void ADoor::OpenDoor()
{
	if (bDoorOpen || !IsValid(OpenDoorAnim))
		return;

	DoorSkeletalMesh->PlayAnimation(OpenDoorAnim, false);
	if (IsValid(OpenDoorSound))
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenDoorSound, DoorSkeletalMesh->GetSocketLocation(DoorSoundSocketName));
	else
		UE_LOG(LogTemp, Warning, TEXT("Open Door Sound is nullptr in Door!"));

	bDoorOpen = true;

	GetWorldTimerManager().SetTimer(CloseAfterInactivityHandle, this, &ADoor::CloseDoorsAfterInactivity, CloseDoorAfterInactivityTime, false);
}

void ADoor::CloseDoor()
{
	if (!bDoorOpen || !IsValid(CloseDoorAnim))
		return;

	GetWorldTimerManager().ClearTimer(CloseAfterInactivityHandle);

	DoorSkeletalMesh->PlayAnimation(CloseDoorAnim, false);

	if (IsValid(CloseDoorSound))
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CloseDoorSound, DoorSkeletalMesh->GetSocketLocation(DoorSoundSocketName));
	else
		UE_LOG(LogTemp, Warning, TEXT("Close Door Sound is nullptr in Door!"));

	bDoorOpen = false;
}

void ADoor::CloseDoorsAfterInactivity()
{
	CloseDoor();

	if (IsValid(DoorPanelWidget))
		DoorPanelWidget->PlayOpenCloseEffects();

	if (IsValid(DoorPanelSecondWidget))
		DoorPanelSecondWidget->PlayOpenCloseEffects();
}

UDoorPanelWidget* ADoor::GetOtherPanelWidget(UDoorPanelWidget* PanelActivatedByPlayer)
{
	if (DoorPanelWidget == PanelActivatedByPlayer)
	{
		return DoorPanelSecondWidget;
	}
	else if (DoorPanelSecondWidget == PanelActivatedByPlayer)
	{
		return DoorPanelWidget;
	}

	return nullptr;
}

void ADoor::SetUpDoorPanels()
{
	DoorPanelWidget = Cast<UDoorPanelWidget>(DoorPanelWidgetComponent->GetUserWidgetObject());
	
	if (IsValid(DoorPanelWidget))
	{
		DoorPanelWidget->SetActorToUnlock(this);

		if (bUsePinCode)
			DoorPanelWidget->ChangeToUsePin(PinCode);
	}

	DoorPanelSecondWidget = Cast<UDoorPanelWidget>(DoorPanelSecondWidgetComponent->GetUserWidgetObject());

	if (IsValid(DoorPanelSecondWidget))
	{
		DoorPanelSecondWidget->SetActorToUnlock(this);

		if (bUsePinCode)
			DoorPanelSecondWidget->ChangeToUsePin(PinCode);
	}
}

void ADoor::PinCorrect()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void ADoor::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1)
	{
		StopUsingPin();

		if (IsValid(DoorPanelWidget))
			DoorPanelWidget->RestartDoorPanelWidget();
		if (IsValid(DoorPanelSecondWidget))
			DoorPanelSecondWidget->RestartDoorPanelWidget();

	}
}

void ADoor::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void ADoor::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (SavedCustomData.ObjectState != 1)
		return;

	SetUpDoorPanels();

	if (bDoorOpen)
	{
		GetWorldTimerManager().ClearTimer(CloseAfterInactivityHandle);

		DoorSkeletalMesh->PlayAnimation(CloseDoorAnim, false);

		if (IsValid(DoorPanelWidget))
			DoorPanelWidget->ChangeDoorStatus();
		if (IsValid(DoorPanelSecondWidget))
			DoorPanelSecondWidget->ChangeDoorStatus();

		bDoorOpen = false;
	}
}

void ADoor::StopUsingPin()
{
	if (IsValid(DoorPanelWidget))
		DoorPanelWidget->PinIsCorrect(false);

	if (IsValid(DoorPanelSecondWidget))
		DoorPanelSecondWidget->PinIsCorrect(false);
}

