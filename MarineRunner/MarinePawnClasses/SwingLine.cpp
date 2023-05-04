// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingLine.h"
#include "NiagaraComponent.h"

// Sets default values
ASwingLine::ASwingLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LineTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LineTrail"));
	RootComponent = LineTrail;

}

// Called when the game starts or when spawned
void ASwingLine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASwingLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LineInterp();

}

void ASwingLine::LineInterp()
{
	if (bCanTick == false) return;

	if (TimeElapsed < LineTime)
	{
		FVector LerpLocation = FMath::Lerp(GetActorLocation(), HookLocation, TimeElapsed / LineTime);
		SetActorLocation(LerpLocation);
	}
	else if (TimeElapsed > 10.f) Destroy();
	TimeElapsed += GetWorld()->GetDeltaSeconds();
}

