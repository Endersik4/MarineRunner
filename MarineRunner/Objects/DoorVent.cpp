// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/DoorVent.h"
#include "Components/StaticMeshComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "Curves/CurveVector.h"

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
	if (bVentDoorsBasedOnPhysics == false)
		return;

	DoorVentMesh->SetSimulatePhysics(true);
}

void ADoorVent::TakeItem(AMarineCharacter* Character, bool& bIsItWeapon)
{
	if (IsValid(Character) == false || bIsOpen == true)
		return;

	PlayOpenDoorVentLocTimeline();
	bIsOpen = true;
}

void ADoorVent::PlayOpenDoorVentLocTimeline()
{
	FOnTimelineVector TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("OnOpenDoorVentLocProgress"));
	OpenDoorVentLocTimeline.AddInterpVector(DoorVentOpenLocationCurve, TimelineProgress);

	OpenDoorVentLocTimeline.PlayFromStart();
}

void ADoorVent::OnOpenDoorVentLocProgress(FVector VectorValue)
{
	AddActorLocalOffset(VectorValue);

	if (DoorVentOpenRotationCurve == nullptr)
		return;

	const FVector & OpenRotationCurveValues = DoorVentOpenRotationCurve->GetVectorValue(OpenDoorVentLocTimeline.GetPlaybackPosition());
	FRotator AddRotation = FRotator(OpenRotationCurveValues.Y, OpenRotationCurveValues.Z, OpenRotationCurveValues.X);
	AddActorLocalRotation(AddRotation);
}

void ADoorVent::ItemHover(UHUDWidget* MarineHUDWidget)
{
	if (bIsOpen == true)
		return;

	DoorVentMesh->SetRenderCustomDepth(true);
	bIsHovered = true;

	if (IsValid(MarineHUDWidget) == false)
		return;

	MarineHUDWidget->SetItemHoverInformations(DoorVentName, DoorVentDesc, DoorVentIcon);
}

void ADoorVent::ItemUnHover(UHUDWidget* MarineHUDWidget)
{
	if (bIsOpen == true && bIsHovered == false)
		return;

	bIsHovered = false;

	DoorVentMesh->SetRenderCustomDepth(false);

	if (IsValid(MarineHUDWidget) == false) 
		return;

	MarineHUDWidget->PlayAppearAnimForItemHover(false);
}