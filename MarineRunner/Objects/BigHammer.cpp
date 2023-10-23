// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/BigHammer.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/InteractInterface.h"

// Sets default values
ABigHammer::ABigHammer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>("HammerMesh");
	RootComponent = HammerMesh;

	HammerBoxComponent = CreateDefaultSubobject<UBoxComponent>("HammerBoxComponent");
	HammerBoxComponent->SetupAttachment(HammerMesh);
	HammerBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HammerBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ABigHammer::BeginPlay()
{
	Super::BeginPlay();

	HammerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABigHammer::OnBoxBeginOverlap);

	SetupMoveTimeline();
}

// Called every frame
void ABigHammer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveObjectTimeline.TickTimeline(DeltaTime);
}

void ABigHammer::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) == false)
		return;

	IInteractInterface* InteractInterface = Cast<IInteractInterface>(OtherActor);
	if (InteractInterface == nullptr)
		return;

	InteractInterface->ApplyDamage(1000.f, 0.f, SweepResult, nullptr);
}

void ABigHammer::MoveHammerTimelineProgresss(float Value)
{
	FVector ActorLocation = GetActorLocation();
	ActorLocation.Z = Value;
	SetActorLocation(ActorLocation);
}

void ABigHammer::SetupMoveTimeline()
{
	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("MoveHammerTimelineProgresss"));
	MoveObjectTimeline.AddInterpFloat(CurveZLocation, TimelineProgress);
	MoveObjectTimeline.SetLooping(true);
	
	MoveObjectTimeline.PlayFromStart();
}

