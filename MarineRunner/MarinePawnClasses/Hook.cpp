// Copyright Adam Bartela.All Rights Reserved

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

	HookActiveSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HookActiveSphere"));
	RootComponent = HookActiveSphere;

	HookActiveSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HookActiveSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HookActiveSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(HookActiveSphere);
	HookMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	HookStateFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("HookStateFlipBook"));
	HookStateFlipBook->SetupAttachment(HookMesh);
	HookStateFlipBook->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	OriginalHookStateScale = HookStateFlipBook->GetComponentScale();
	HookActiveSphere->OnComponentBeginOverlap.AddDynamic(this, &AHook::OnCheckSphereBeginOverlap);
	HookActiveSphere->OnComponentEndOverlap.AddDynamic(this, &AHook::OnCheckSphereEndOverlap);

	GetWorld()->GetTimerManager().SetTimer(HookVisibleHandle, this, &AHook::HideFlipbookIfItIsNotVisible, CheckIfHookIsVisibleInterval, true);
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
	if (GetWorld()->GetTimerManager().IsTimerActive(HookPressedHandle))
		return;
	
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

	ChangeToIdleAnim();
}

void AHook::ChangeHookFlipbookScale(float Delta)
{
	if (!bHookActive|| !IsValid(PlayerInRange))
		return;

	if (FVector::Distance(PlayerInRange->GetActorLocation(), HookStateFlipBook->GetComponentLocation()) > StartChangingScaleDistance
		|| FVector::Distance(PlayerInRange->GetActorLocation(), HookStateFlipBook->GetComponentLocation()) < EndChangingScaleDistance)
		return;

	float Alpha = FVector::Distance(PlayerInRange->GetActorLocation(), HookStateFlipBook->GetComponentLocation()) / FMath::Abs(StartChangingScaleDistance - EndChangingScaleDistance);
	FVector NewScale = FMath::Lerp(MinHookFlipbookScale, OriginalHookStateScale, Alpha);
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
	if (!bHookActive|| !IsValid(PlayerInRange))
		return;

	FHitResult HitResult;
	bool bIsSomethingBlockingView = GetWorld()->LineTraceSingleByChannel(HitResult, HookStateFlipBook->GetComponentLocation(), PlayerInRange->GetActorLocation(), ECC_Visibility);
	
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
