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
	CheckSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(CheckSphere);
	HookMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	HookStateFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("HookStateFlipBook"));
	HookStateFlipBook->SetupAttachment(HookMesh);
	HookStateFlipBook->SetCollisionProfileName(FName("NoCollision"));

	Tags.Add(TEXT("Hook"));
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	OriginalHookStateScale = HookStateFlipBook->GetComponentScale();
	CheckSphere->OnComponentBeginOverlap.AddDynamic(this, &AHook::OnCheckSphereBeginOverlap);
	CheckSphere->OnComponentEndOverlap.AddDynamic(this, &AHook::OnCheckSphereEndOverlap);
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ChangeHookFlipbookScale(DeltaTime);
	HookFlipbookLookAtThePlayer(DeltaTime);
	HideFlipbookIfItIsNotVisible();
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

	if (bPlayerInRange == false) 
		ChangeToIdleAnim();
}

void AHook::ChangeToIdleAnim()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(HookPressedHandle) == true)
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
	if (bHookActive == false || IsValid(PlayerInRange) == false)
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
	if (bHookActive == false || IsValid(PlayerInRange) == false)
		return;

	FRotator LookAtThePlayerRotation = UKismetMathLibrary::FindLookAtRotation(HookStateFlipBook->GetComponentLocation(), PlayerInRange->GetActorLocation());
	Swap(LookAtThePlayerRotation.Pitch, LookAtThePlayerRotation.Roll);
	LookAtThePlayerRotation.Yaw += 90.f;
	HookStateFlipBook->SetRelativeRotation(LookAtThePlayerRotation);
}

void AHook::HideFlipbookIfItIsNotVisible()
{
	if (bHookActive == false || IsValid(PlayerInRange) == false)
		return;

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, HookStateFlipBook->GetComponentLocation(), PlayerInRange->GetActorLocation(), ECC_Visibility);
	if (bHit == false && HookStateFlipBook->IsVisible() == false)
	{
		HookStateFlipBook->SetVisibility(true);
	}
	else if (bHit == true && HookStateFlipBook->IsVisible() == true)
	{
		HookStateFlipBook->SetVisibility(false);
	}
}

void AHook::ResetHookStateFlipbookScale()
{
	HookStateFlipBook->SetWorldScale3D(OriginalHookStateScale);
}
