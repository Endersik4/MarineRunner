// Copyright Adam Bartela.All Rights Reserved

#include "SwingLine.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASwingLine::ASwingLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwingLineTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SwingLineTrail"));
	RootComponent = SwingLineTrail;
}

// Called when the game starts or when spawned
void ASwingLine::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeSpan);
}

// Called every frame
void ASwingLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToHookLocation(DeltaTime);
}

void ASwingLine::StartMovingToHookLocation(const FVector& NewHookLocation, float NewSwingToHookLocationTime)
{
	bStartMovingToHookLocation = true;
	HookLocation = NewHookLocation;
	SwingToHookLocationTime = NewSwingToHookLocationTime;
	InitialSwingLineLocation = GetActorLocation();
}

void ASwingLine::MoveToHookLocation(float Delta)
{
	if (!bStartMovingToHookLocation) 
		return;

	if (SwingToHookLocationTimeElapsed < SwingToHookLocationTime)
	{
		const FVector& NewLocaton = FMath::Lerp(InitialSwingLineLocation, HookLocation, SwingToHookLocationTimeElapsed / SwingToHookLocationTime);
		SetActorLocation(NewLocaton);
	}
	else
	{
		bStartMovingToHookLocation = false;
	}

	SwingToHookLocationTimeElapsed += Delta;
}

