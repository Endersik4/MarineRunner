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

	CheckSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CheckSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CheckSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(CheckSphere);

	HookStateFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("HookStateFlipBook"));
	HookStateFlipBook->SetupAttachment(HookMesh);
	HookStateFlipBook->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHook::StartHookCooldown()
{
	bCanGrabTheHook = false;
	CheckSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FTimerHandle HookPressedHandle;
	GetWorld()->GetTimerManager().SetTimer(HookPressedHandle, this, &AHook::DelayForGrabbingTheHook, HookCooldownTime, false);
}

void AHook::ActivateHook(bool bActive)
{
	HookStateFlipBook->SetVisibility(bActive);
	ChangeToIdleAnim();
}

void AHook::ChangeToIdleAnim()
{
	HookStateFlipBook->PlayFromStart();

	HookStateFlipBook->SetLooping(true);
	HookStateFlipBook->SetFlipbook(HookIdleFlipBook);
}

void AHook::ChangeToPlayerInRangeAnim()
{
	HookStateFlipBook->SetLooping(false);
	HookStateFlipBook->PlayFromStart();

	HookStateFlipBook->SetFlipbook(HookActivateFlipBook);
}

void AHook::DelayForGrabbingTheHook()
{
	bCanGrabTheHook = true;
	CheckSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
