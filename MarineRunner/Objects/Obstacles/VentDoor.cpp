// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Obstacles/VentDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveVector.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

ADoorVent::ADoorVent()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorVentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Vent Mesh"));
	RootComponent = DoorVentMesh;

	DoorVentMesh->SetGenerateOverlapEvents(false);
	DoorVentMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	DoorVentMesh->bRenderCustomDepth = false; //Its for outline material 
}

void ADoorVent::BeginPlay()
{
	Super::BeginPlay();

	OnActorHit.AddDynamic(this, &ADoorVent::OnHit);
}

void ADoorVent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpenDoorVentLocTimeline.TickTimeline(DeltaTime);
}

void ADoorVent::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bVentDoorsBasedOnPhysics && !DoorVentMesh->IsSimulatingPhysics())
		return;

	DoorVentMesh->SetSimulatePhysics(true);
	SaveCurrentStateOfVent(3, FTransform(GetActorRotation(), GetActorLocation()));
}

void ADoorVent::TakeItem(AMarineCharacter* Character)
{
	if (!IsValid(Character) || bIsOpen || DoorVentMesh->IsSimulatingPhysics())
		return;

	if (TurnOnPhysicsInsteadOfTimeline)
	{
		DoorVentMesh->SetSimulatePhysics(true);
		SaveCurrentStateOfVent(3, FTransform(GetActorRotation(), GetActorLocation()));
	}
	else
	{
		PlayOpenDoorVentLocTimeline();
		bIsOpen = true;
	}
}

void ADoorVent::PlayOpenDoorVentLocTimeline()
{
	FOnTimelineVector TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("OnOpenDoorVentLocProgress"));
	OpenDoorVentLocTimeline.AddInterpVector(DoorVentOpenLocationCurve, TimelineProgress);

	FOnTimelineEventStatic TimelineFinished;
	TimelineFinished.BindUFunction(this, FName("OnOpenDoorVentLocFinished"));
	OpenDoorVentLocTimeline.SetTimelineFinishedFunc(TimelineFinished);

	OpenDoorVentLocTimeline.PlayFromStart();
}

void ADoorVent::OnOpenDoorVentLocProgress(FVector VectorValue)
{
	if (bAddCurveValuesToVectors)
	{
		AddActorLocalOffset(VectorValue);

		if (DoorVentOpenRotationCurve) 
			AddActorLocalRotation(GetRotationFromDoorVentOpenCurve());
	}
	else
	{
		SetActorLocation(VectorValue);

		if (DoorVentOpenRotationCurve) 
			SetActorRotation(GetRotationFromDoorVentOpenCurve());
	}
}

FRotator ADoorVent::GetRotationFromDoorVentOpenCurve()
{
	const FVector& OpenRotationCurveValues = DoorVentOpenRotationCurve->GetVectorValue(OpenDoorVentLocTimeline.GetPlaybackPosition());
	return FRotator(OpenRotationCurveValues.Y, OpenRotationCurveValues.Z, OpenRotationCurveValues.X);
}

void ADoorVent::OnOpenDoorVentLocFinished()
{
	DoorVentMesh->SetSimulatePhysics(true);
	SaveCurrentStateOfVent(3, FTransform(GetActorRotation(), GetActorLocation()));
	OpenDoorVentLocTimeline.Stop();
}

void ADoorVent::ItemHover(AMarineCharacter* Character)
{
	if (bIsOpen || DoorVentMesh->IsSimulatingPhysics())
		return;

	DoorVentMesh->SetRenderCustomDepth(true);
	bIsHovered = true;

	if (!IsValid(Character->GetHudWidget()))
		return;

	Character->GetHudWidget()->SetItemHoverInformations(DoorVentName, DoorVentDesc, DoorVentIcon);
}

void ADoorVent::ItemUnHover(AMarineCharacter* Character)
{
	if (!bIsHovered)
		return;

	bIsHovered = false;

	DoorVentMesh->SetRenderCustomDepth(false);

	if (!IsValid(Character->GetHudWidget()))
		return;

	Character->GetHudWidget()->PlayAppearAnimForItemHover(false);
}

void ADoorVent::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	DoorVentMesh->SetSimulatePhysics(true);

	SetActorLocation(SavedCustomData.ObjectTransform.GetLocation());
	SetActorRotation(SavedCustomData.ObjectTransform.GetRotation());
}

void ADoorVent::SaveCurrentStateOfVent(int32 CurrentState, FTransform ActoTransform)
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	FCustomDataSaved CurrentStateSaved = FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, CurrentState);
	CurrentStateSaved.ObjectTransform = ActoTransform;
	SavedDataObject->AddCustomSaveData(CurrentUniqueID, CurrentStateSaved);
}

void ADoorVent::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (SavedCustomData.ObjectState == 2)
		return;

	FCustomDataSaved UpdatedData = SavedCustomData;
	UpdatedData.ObjectTransform = FTransform(GetActorRotation(), GetActorLocation());
	SavedDataObject->AddCustomSaveData(IDkey, UpdatedData);
}

void ADoorVent::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}
