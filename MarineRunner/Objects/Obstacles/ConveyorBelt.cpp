// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Obstacles/ConveyorBelt.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AConveyorBelt::AConveyorBelt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConveyorBeltInstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ConveyorBeltInstancedMesh"));
	RootComponent = ConveyorBeltInstancedMesh;

	MoveActorBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MoveActorBoxComponent"));
	MoveActorBoxComponent->SetupAttachment(RootComponent);
	MoveActorBoxComponent->SetCollisionProfileName(FName("ConveyorBeltBox"));
}

// Called when the game starts or when spawned
void AConveyorBelt::BeginPlay()
{
	Super::BeginPlay();
	
	MoveActorBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AConveyorBelt::OnMoveActorBoxBeginOverlap);
	MoveActorBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AConveyorBelt::OnMoveActorBoxEndOverlap);
}

// Called every frame
void AConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveActors(DeltaTime);
}

void AConveyorBelt::GenerateInstancedMeshesForConveyorBelt()
{
	ConveyorBeltInstancedMesh->ClearInstances();

	for (int32 i = 0; i != ConveyorBeltMeshAmount; i++)
	{
		const FVector& InstancedMeshRelativeLocation = InstancedMeshDirection * ConveyorBeltMeshOffset * i;
		ConveyorBeltInstancedMesh->AddInstance(FTransform(InstancedMeshRelativeLocation));
	}

	CalculateBoxExtent();

	CalculateBoxRelativeLocation();
}

void AConveyorBelt::CalculateBoxExtent()
{
	FVector NewBoxExtent = MoveActorBoxComponent->GetUnscaledBoxExtent() - MoveActorBoxComponent->GetUnscaledBoxExtent() * InstancedMeshDirection;
	NewBoxExtent += InstancedMeshDirection * (ConveyorBeltMeshOffset / 2) * (ConveyorBeltMeshAmount);
	MoveActorBoxComponent->SetBoxExtent(NewBoxExtent);
}

void AConveyorBelt::CalculateBoxRelativeLocation()
{
	FVector NewBoxRelativeLocation = MoveActorBoxComponent->GetRelativeLocation() - (MoveActorBoxComponent->GetRelativeLocation() * InstancedMeshDirection);
	NewBoxRelativeLocation += InstancedMeshDirection * ((InstancedMeshDirection * ConveyorBeltMeshOffset * (ConveyorBeltMeshAmount - 1)) / 2);
	MoveActorBoxComponent->SetRelativeLocation(NewBoxRelativeLocation);
}

void AConveyorBelt::MoveActors(float Delta)
{
	if (!bConveyorBeltActive)
		return;
	
	// make sure to not remove actor while array is iterating
	TArray<TObjectPtr<AActor>> SaveActorsOnConveyorBelt = ActorsOnConveyorBelt;
	for (TObjectPtr<AActor> CurrentActor : SaveActorsOnConveyorBelt)
	{
		if (!ensureMsgf(IsValid(CurrentActor), TEXT("Actor To Move in Conveyor Belt is nullptr")))
			continue;

		if (!IsValid(CurrentActor))
			continue;

		const FVector& Offset = GetActorForwardVector() * Delta * SpeedOfActorOnConveyorBelt;
		CurrentActor->AddActorWorldOffset(Offset);
	}
}

void AConveyorBelt::OnMoveActorBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ActorsOnConveyorBelt.AddUnique(OtherActor);
}

void AConveyorBelt::OnMoveActorBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorsOnConveyorBelt.Remove(OtherActor);
}

