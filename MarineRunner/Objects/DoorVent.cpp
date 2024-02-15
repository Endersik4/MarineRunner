// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/DoorVent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveVector.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Objects/SavedDataObject.h"

// Sets default values
ADoorVent::ADoorVent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorVentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Vent Mesh"));
	RootComponent = DoorVentMesh;

	DoorVentMesh->SetGenerateOverlapEvents(false);
	DoorVentMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	DoorVentMesh->bRenderCustomDepth = false; //Its for outline material 
}

// Called when the game starts or when spawned
void ADoorVent::BeginPlay()
{
	Super::BeginPlay();

	OnActorHit.AddDynamic(this, &ADoorVent::OnHit);
}

// Called every frame
void ADoorVent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpenDoorVentLocTimeline.TickTimeline(DeltaTime);
}

void ADoorVent::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bVentDoorsBasedOnPhysics == false && DoorVentMesh->IsSimulatingPhysics() == false)
		return;

	DoorVentMesh->SetSimulatePhysics(true);
	SaveCurrentStateOfVent(3, FTransform(GetActorRotation(), GetActorLocation()));
}

void ADoorVent::TakeItem(AMarineCharacter* Character)
{
	if (IsValid(Character) == false || bIsOpen == true || DoorVentMesh->IsSimulatingPhysics() == true)
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
	
	SaveCurrentStateOfVent(2, FTransform(FinalRotation, FinalLocation));
}

void ADoorVent::OnOpenDoorVentLocProgress(FVector VectorValue)
{
	if (bAddCurveValuesToVectors == true)
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
}

void ADoorVent::ItemHover(AMarineCharacter* Character)
{
	if (bIsOpen == true || DoorVentMesh->IsSimulatingPhysics() == true)
		return;

	DoorVentMesh->SetRenderCustomDepth(true);
	bIsHovered = true;

	if (IsValid(Character->GetHudWidget()) == false)
		return;

	Character->GetHudWidget()->SetItemHoverInformations(DoorVentName, DoorVentDesc, DoorVentIcon);
}

void ADoorVent::ItemUnHover(AMarineCharacter* Character)
{
	if (bIsHovered == false)
		return;

	bIsHovered = false;

	DoorVentMesh->SetRenderCustomDepth(false);

	if (IsValid(Character->GetHudWidget()) == false)
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
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
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

	SavedDataObject->RemoveCustomSaveData(IDkey);
	FCustomDataSaved UpdatedData = SavedCustomData;
	UpdatedData.ObjectTransform = FTransform(GetActorRotation(), GetActorLocation());
	SavedDataObject->AddCustomSaveData(IDkey, UpdatedData);
}

void ADoorVent::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}
