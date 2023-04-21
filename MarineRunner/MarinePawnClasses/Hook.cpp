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
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerPawn();
	Radius = CheckSphere->GetScaledSphereRadius();
	
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MarinePawn)
	{
		float Distance = FVector::Distance(MarinePawn->GetActorLocation(), GetActorLocation());
		if (Distance < (Radius + 300.f))
		{
			if (Distance - 500.f > 150.f) CheckSphere->SetSphereRadius(Distance - 500.f);
		}
		else CheckSphere->SetSphereRadius(Radius);
	}

}

void AHook::HookActivate()
{
	HookMesh->SetMaterial(0, M_PlayerInRange);
}

void AHook::HookInactiv()
{
	HookMesh->SetMaterial(0, M_PlayerOutRange);
}

void AHook::SetPlayerPawn()
{
	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if (MarinePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("Hook doesnt have PlayerPawn set"));
}

