// Fill out your copyright notice in the Description page of Project Settings.


#include "Hook.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values
AHook::AHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	RootComponent = HookMesh;

	CheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CheckSphere"));
	CheckSphere->SetupAttachment(HookMesh);

	Tags.Add(TEXT("Hook"));
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	CopyOfSphereRadius = CheckSphere->GetUnscaledSphereRadius();
	SetPlayerPawn();
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHook::HookPressed()
{
	bCanGrabTheHook = false;
	CheckSphere->SetSphereRadius(0.f);
	FTimerHandle HookPressedHandle;
	GetWorld()->GetTimerManager().SetTimer(HookPressedHandle, this, &AHook::DelayForGrabbingTheHook, 1.5f, false);
}

void AHook::HookActivate()
{
	HookMesh->SetMaterial(0, M_PlayerInRange);
}

void AHook::HookInactiv()
{
	HookMesh->SetMaterial(0, M_PlayerOutRange);
}

void AHook::DelayForGrabbingTheHook()
{
	bCanGrabTheHook = true;
	CheckSphere->SetSphereRadius(CopyOfSphereRadius);
}

void AHook::SetPlayerPawn()
{
	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if (MarinePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("Hook doesnt have PlayerPawn set"));
}

