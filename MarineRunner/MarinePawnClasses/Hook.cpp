// Fill out your copyright notice in the Description page of Project Settings.


#include "Hook.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PaperFlipbookComponent.h"

// Sets default values
AHook::AHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CheckSphere"));
	RootComponent = CheckSphere;

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(CheckSphere);

	HookStateFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("HookStateFlipBook"));
	HookStateFlipBook->SetupAttachment(HookMesh);
	HookStateFlipBook->SetCollisionProfileName(FName("NoCollision"));

	Tags.Add(TEXT("Hook"));
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	OriginalSphereRadius = CheckSphere->GetUnscaledSphereRadius();
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHook::StartHookCooldown()
{
	bCanGrabTheHook = false;
	CheckSphere->SetSphereRadius(0.f);
	FTimerHandle HookPressedHandle;
	GetWorld()->GetTimerManager().SetTimer(HookPressedHandle, this, &AHook::DelayForGrabbingTheHook, 1.5f, false);
}

void AHook::HookActivate(bool bActive)
{
	if (bActive)
	{
		HookMesh->SetMaterial(PlayerInRangeIndexMaterial, M_PlayerInRange);
		HookStateFlipBook->SetFlipbook(HookActivateFlipBook);
	}
	else
	{
		HookMesh->SetMaterial(PlayerInRangeIndexMaterial, M_PlayerOutRange);
		HookStateFlipBook->SetFlipbook(HookIdleFlipBook);
	}

}
void AHook::DelayForGrabbingTheHook()
{
	bCanGrabTheHook = true;
	CheckSphere->SetSphereRadius(OriginalSphereRadius);
}