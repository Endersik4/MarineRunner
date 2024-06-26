// Copyright Adam Bartela.All Rights Reserved

#include "Hook.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PaperFlipbookComponent.h"

AHook::AHook()
{
	PrimaryActorTick.bCanEverTick = true;

	HookActiveSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HookActiveSphere"));
	RootComponent = HookActiveSphere;

	HookActiveSphere->SetCollisionProfileName(FName(TEXT("DetectOnlyPlayer")));

	GrabHookBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Grab Hook Box Component"));
	GrabHookBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GrabHookBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	GrabHookBoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	GrabHookBoxComponent->SetupAttachment(RootComponent);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(RootComponent);
	HookMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HookMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	HookMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	HookStateFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("HookStateFlipBook"));
	HookStateFlipBook->SetupAttachment(HookMesh);
	HookStateFlipBook->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	OriginalHookStateScale = HookStateFlipBook->GetComponentScale();

	HookActiveSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHook::OnCheckSphereBeginOverlap);
	HookActiveSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AHook::OnCheckSphereEndOverlap);

	GetWorld()->GetTimerManager().SetTimer(HookVisibleHandle, this, &AHook::HideFlipbookIfItIsNotVisible, CheckIfHookIsVisibleInterval, true);

	PlayerInRangeWhenGameStarted();
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChangeHookFlipbookScale(DeltaTime);
	HookFlipbookLookAtThePlayer(DeltaTime);
}

void AHook::OnCheckSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ActivateHook(true);
	PlayerInRange = OtherActor;
}

void AHook::OnCheckSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActivateHook(false);
	PlayerInRange = nullptr;
	ResetHookStateFlipbookScale();
}

void AHook::StartHookCooldown()
{
	bCanGrabTheHook = false;

	GetWorld()->GetTimerManager().SetTimer(HookPressedHandle, this, &AHook::DelayForGrabbingTheHook, HookCooldownTime, false);
}

void AHook::ActivateHook(bool bActive)
{
	bHookActive = bActive;
	HookStateFlipBook->SetVisibility(bActive);

	if (!bPlayerInRange) 
		ChangeToIdleAnim();
}

void AHook::ChangeToIdleAnim()
{
	HookStateFlipBook->PlayFromStart();
	HookStateFlipBook->SetLooping(true);
	HookStateFlipBook->SetFlipbook(HookIdleFlipBook);

	bPlayerInRange = false;
}

void AHook::ChangeToPlayerInRangeAnim()
{
	bPlayerInRange = true;

	HookStateFlipBook->SetLooping(false);
	HookStateFlipBook->PlayFromStart();
	HookStateFlipBook->SetFlipbook(HookActivateFlipBook);
}

void AHook::DelayForGrabbingTheHook()
{
	bCanGrabTheHook = true;
}

void AHook::ChangeHookFlipbookScale(float Delta)
{
	if (!bHookActive || !IsValid(PlayerInRange))
		return;

	const float& DistanceToPlayer = FVector::Distance(PlayerInRange->GetActorLocation(), HookStateFlipBook->GetComponentLocation());
	if (DistanceToPlayer > StartChangingScaleDistance || DistanceToPlayer < EndChangingScaleDistance)
		return;

	const float& Alpha = FVector::Distance(PlayerInRange->GetActorLocation(), HookStateFlipBook->GetComponentLocation()) / FMath::Abs(StartChangingScaleDistance - EndChangingScaleDistance);
	const FVector& NewScale = FMath::Lerp(MinHookFlipbookScale, OriginalHookStateScale, Alpha);
	HookStateFlipBook->SetWorldScale3D(NewScale);
}

void AHook::HookFlipbookLookAtThePlayer(float Delta)
{
	if (!bHookActive|| !IsValid(PlayerInRange))
		return;

	FRotator LookAtThePlayerRotation = UKismetMathLibrary::FindLookAtRotation(HookStateFlipBook->GetComponentLocation(), PlayerInRange->GetActorLocation());
	Swap(LookAtThePlayerRotation.Pitch, LookAtThePlayerRotation.Roll);
	LookAtThePlayerRotation += RotationOffsetToLookAtThePlayer;

	HookStateFlipBook->SetRelativeRotation(LookAtThePlayerRotation);
}

void AHook::HideFlipbookIfItIsNotVisible()
{
	if (!bHookActive || !IsValid(PlayerInRange))
		return;

	FHitResult HitResult;
	const bool bIsSomethingBlockingView = GetWorld()->LineTraceSingleByChannel(HitResult, HookStateFlipBook->GetComponentLocation(), PlayerInRange->GetActorLocation(), ECC_Visibility);
	
	if (!bIsSomethingBlockingView && !HookStateFlipBook->IsVisible())
	{
		HookStateFlipBook->SetVisibility(true);
	}
	else if (bIsSomethingBlockingView && HookStateFlipBook->IsVisible())
	{
		HookStateFlipBook->SetVisibility(false);
	}
}

void AHook::ResetHookStateFlipbookScale()
{
	HookStateFlipBook->SetWorldScale3D(OriginalHookStateScale);
}

void AHook::PlayerInRangeWhenGameStarted()
{
	HookActiveSphere->UpdateOverlaps();

	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);
	if (Actors.Num() == 0)
		return;

	ActivateHook(true);
	PlayerInRange = Actors[0];
}
