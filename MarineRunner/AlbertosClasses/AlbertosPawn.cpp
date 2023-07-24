// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"
#include "MarineRunner/Inventory/PickupItem.h"

// Sets default values
AAlbertosPawn::AAlbertosPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AlbertosBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("AlbertosBoxComponent"));
	RootComponent = AlbertosBoxComponent;
	bUseControllerRotationYaw = true;
	AlbertosBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AlbertosBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	AlbertosBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	AlbertosBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	AlbertosFloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("AlbertosFloatingMovement"));

	AlbertosSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AlbertosSkeletalMesh"));
	AlbertosSkeletalMesh->SetupAttachment(AlbertosBoxComponent);
	AlbertosSkeletalMesh->SetSimulatePhysics(false);
	AlbertosSkeletalMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	AlbertosSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	InventoryButtonComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Inventory Button Component"));
	InventoryButtonComponent->SetupAttachment(AlbertosSkeletalMesh);
	InventoryButtonComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InventoryButtonComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	CraftingTableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CraftingTableWidget"));
	CraftingTableWidget->SetupAttachment(AlbertosSkeletalMesh);
	CraftingTableWidget->SetDrawAtDesiredSize(true);

	Hologram_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hologram_1"));
	Hologram_1->SetupAttachment(CraftingTableWidget);
	Hologram_1->SetCollisionProfileName(FName(TEXT("NoCollision")));
	Hologram_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hologram_2"));
	Hologram_2->SetCollisionProfileName(FName(TEXT("NoCollision")));
	Hologram_2->SetupAttachment(CraftingTableWidget);
	Hologram_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// Called when the game starts or when spawned
void AAlbertosPawn::BeginPlay()
{
	Super::BeginPlay();

	AlbertosAI = Cast<AAlbertosAIController>(GetController());
	
	CopyOfMaxSpeed = AlbertosFloatingMovement->GetMaxSpeed();

	UCraftingAlbertosWidget* CraftingWidget = Cast<UCraftingAlbertosWidget>(GetCraftingTableWidget());
	if (CraftingWidget) CraftingWidget->SetAlbertosPawn(this);

	GetWorld()->GetTimerManager().SetTimer(PlayerIsNearHandle, this, &AAlbertosPawn::CheckIfThePlayerIsNear, 0.5f, true);
}

// Called every frame
void AAlbertosPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InterpToFinalPosition(DeltaTime);
	RotateAlbertosTowardsPlayer(DeltaTime);

}

// Called to bind functionality to input
void AAlbertosPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UUserWidget* AAlbertosPawn::GetCraftingTableWidget() const
{
	return CraftingTableWidget->GetUserWidgetObject();
}

#pragma region //////////////// Crafting ////////////////
void AAlbertosPawn::CraftPressed(APickupItem* SpawnedCraftingItem)
{
	if (SpawnedCraftingItem == nullptr) return;
	
	CraftedItem = SpawnedCraftingItem;
	if (CraftedItem->GetItemSettings().Item_CraftScale != FVector(0.f))
	{
		bShouldScaleCraftedItem = true;
		TargetScaleOfCraftedItem = CraftedItem->GetItemSettings().Item_CraftScale;
	}

	if (bIsFrontDoorOpen == false)
	{
		OpenFrontDoor(AlbertosSkeletalMesh);
		bIsFrontDoorOpen = true;
	}

	EnableCraftingAnimation(AlbertosSkeletalMesh);

	CraftedItem->SetDissolveMaterial(OverlayCraftingMaterial);
}

void AAlbertosPawn::CraftingFinished()
{
	EnableCraftingAnimation(AlbertosSkeletalMesh, false, 0.f);

	if (CraftedItem == nullptr) return;

	CraftedItem->ChangeSimulatingPhysics(false);

	bShouldMoveToFinalPosition = true;
	FinalLocation = CraftedItem->GetActorLocation() + GetActorForwardVector() * FVector::Distance(AlbertosSkeletalMesh->GetSocketLocation(FName(TEXT("FinalItemPosition"))), CraftedItem->GetActorLocation());
}
#pragma endregion

#pragma region //////////////////// Inventory /////////////////
void AAlbertosPawn::TakeItem(AMarineCharacter* Character, bool& bIsItWeapon)
{
	bIsItWeapon = false;
	ToggleVisibilityInventory();
	if (CraftingTableWidget->IsVisible()) Character->UpdateAlbertosInventory(true, true);
}

void AAlbertosPawn::ToggleVisibilityInventory(bool bCheckIfHidden)
{
	if (bCheckIfHidden == false)
	{
		CraftingTableWidget->ToggleVisibility();
		Hologram_1->ToggleVisibility();
		Hologram_2->ToggleVisibility();
	}
	else if(CraftingTableWidget->IsVisible())
	{
		ToggleVisibilityInventory();
	}
}
#pragma endregion;

#pragma region ///////////////////////// The Player is Located Near Albertos /////////////////////////

void AAlbertosPawn::CheckIfThePlayerIsNear()
{
	if (AlbertosAI == nullptr) return;

	if (FVector::Distance(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation(), GetActorLocation()) <= ActiveAlbertosRadius)
	{
		if (bPlayerWasClose == true) return;
		
		AlbertosAI->SetCanMove(true);
		AlbertosAI->StopMovement();

		bCanRotateAlbertos = true;

		bPlayerWasClose = true;
	}
	else if (bPlayerWasClose == true && CraftedItem == nullptr)
	{
		AlbertosAI->SetCanMove(false);

		ToggleVisibilityInventory(true);
		if (bIsFrontDoorOpen == true)
		{
			OpenFrontDoor(AlbertosSkeletalMesh, false);
			bIsFrontDoorOpen = false;
		}
		bPlayerWasClose = false;
	}
}

void AAlbertosPawn::RotateAlbertosTowardsPlayer(float Delta)
{
	if (bCanRotateAlbertos == false) return;

	FRotator RotatorLook = GetActorRotation();
	RotatorLook.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()).Yaw;
	FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), RotatorLook, Delta, 8.f);
	SetActorRotation(NewRotation);

	if (FMath::IsNearlyEqual(RotatorLook.Yaw, NewRotation.Yaw, 2.f)) bCanRotateAlbertos = false;
}

#pragma endregion

#pragma region /////////////////////// Crafted Item ///////////////////////////
void AAlbertosPawn::InterpToFinalPosition(float Delta)
{
	if (bShouldMoveToFinalPosition == false || CraftedItem == nullptr) return;

	if (!CraftedItem->GetActorLocation().Equals(FinalLocation, 10.f))
	{
		FVector NewLocation = FMath::VInterpTo(CraftedItem->GetActorLocation(), FinalLocation, Delta, SpeedOfItemAfterCrafting);
		CraftedItem->SetActorLocation(NewLocation);

		if (bShouldScaleCraftedItem == false) return;

		FVector NewScale = FMath::VInterpTo(CraftedItem->GetActorScale3D(), TargetScaleOfCraftedItem, Delta, SpeedOfItemAfterCrafting * 2.2f);
		CraftedItem->SetActorScale3D(NewScale);
	}
	else
	{
		if (bShouldScaleCraftedItem == true) CraftedItem->SetActorScale3D(TargetScaleOfCraftedItem);

		bShouldMoveToFinalPosition = false;
		bShouldScaleCraftedItem = false;

		CraftedItem->ChangeSimulatingPhysics(true);

		CraftedItem->SetCollisionNewResponse(ECC_GameTraceChannel1, ECR_Block);
		CraftedItem->SetCollisionNewResponse(ECC_GameTraceChannel3, ECR_Block);
		CraftedItem = nullptr;

		if (bPlayerWasClose == true) CheckIfThePlayerIsNear();
	}
	
}
#pragma endregion

void AAlbertosPawn::CallAlbertoToThePlayer(FVector PlayerLoc)
{
	if (AlbertosAI == nullptr) return;

	PlayerLoc.Z = GetActorLocation().Z;
	AlbertosAI->CallAlbertosToThePlayer(PlayerLoc);

	ChangeMaxSpeedOfFloatingMovement();
}

void AAlbertosPawn::ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer)
{
	if (bTowardsPlayer == true) AlbertosFloatingMovement->MaxSpeed = MaxSpeedWhenMovingTowardsPlayer;
	else AlbertosFloatingMovement->MaxSpeed = CopyOfMaxSpeed;
	
}
