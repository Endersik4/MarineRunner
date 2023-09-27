// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Objects/Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckpointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointBox"));
	RootComponent = CheckpointBox;

	CheckpointBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnCheckpointBoxBeginOverlap);
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::DisableCheckpoint()
{
	bSaved = true;
	CheckpointBox->SetCollisionProfileName(FName(TEXT("NoCollision")));
}

void ACheckpoint::OnCheckpointBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player") || bSaved == true) return;

	// Prevent where DisableCheckpoint function is done After overlapping the player
	if (UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) < MaxGameTimeToSaveGame)
		return;

	AMarineCharacter* MarinePawn = Cast<AMarineCharacter>(OtherActor);
	if (MarinePawn == nullptr) return;

	MarinePawn->CallSaveGame(this);
	bSaved = true;
}

