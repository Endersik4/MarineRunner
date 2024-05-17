// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Obstacles/DeadlyMovableBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/DamageInterface.h"

ABigHammer::ABigHammer()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("Root Scene Component");
	RootComponent = RootSceneComponent;

	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>("HammerMesh");
	HammerMesh->SetupAttachment(RootSceneComponent);

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
	if (!IsValid(OtherActor))
		return;

	IDamageInterface* InteractInterface = Cast<IDamageInterface>(OtherActor);
	if (!InteractInterface)
		return;

	InteractInterface->ApplyDamage(DamageToApply, ImpulseForceToApply, SweepResult, nullptr);
}

void ABigHammer::MoveHammerTimelineProgress(float Value)
{
	HammerMesh->SetRelativeLocation(DirectorToApplyCurve * Value);
}

void ABigHammer::SetupMoveTimeline()
{
	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("MoveHammerTimelineProgress"));
	MoveObjectTimeline.AddInterpFloat(CurveZLocation, TimelineProgress);

	FOnTimelineEventStatic TimelineEnd;
	TimelineEnd.BindUFunction(this, FName("MoveHammerTimelineFinished"));
	MoveObjectTimeline.SetTimelineFinishedFunc(TimelineEnd);

	MoveObjectTimeline.PlayFromStart();

	GetWorldTimerManager().SetTimer(HitSoundHandle, this, &ABigHammer::PlayHitSound, TimeToPlayHitSound, false);
}

void ABigHammer::PlayHitSound()
{
	if (IsValid(HitSound))
		UGameplayStatics::SpawnSoundAttached(HitSound, HammerMesh);
	else
		UE_LOG(LogTemp, Warning, TEXT("Hit Sound is nullptr in DeadlyMovableBox!"));
}

void ABigHammer::MoveHammerTimelineFinished()
{
	SetupMoveTimeline();
}

