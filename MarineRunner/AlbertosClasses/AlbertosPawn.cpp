// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"
#include "MarineRunner/AlbertosClasses/Components/CraftItemAlbertosComponent.h"
#include "MarineRunner/AlbertosClasses/Components/CraftingWidgetAnimationComponent.h"
#include "MarineRunner/AlbertosClasses/Components/PlayerIsNearAlbertosComponent.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Objects/SavedDataObject.h"

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

	CraftItemAlbertosComponent = CreateDefaultSubobject<UCraftItemAlbertosComponent>(TEXT("Craft Item Albertos Component"));
	CraftingWidgetAnimationComponent = CreateDefaultSubobject<UCraftingWidgetAnimationComponent>(TEXT("Crafting Widget Animation Component"));
	PlayerIsNearAlbertosComponent = CreateDefaultSubobject<UPlayerIsNearAlbertosComponent>(TEXT("Player Is Near Albertos Component"));

	CraftingTableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CraftingTableWidget"));
	CraftingTableWidget->SetupAttachment(AlbertosSkeletalMesh);
	CraftingTableWidget->SetDrawAtDesiredSize(true);

	HologramMeshEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HologramMeshEffect"));
	HologramMeshEffect->SetCollisionProfileName(FName(TEXT("NoCollision")));
	HologramMeshEffect->SetupAttachment(CraftingTableWidget);
	HologramMeshEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DissolveBox_Left = CreateOptionalDefaultSubobject<UChildActorComponent>(TEXT("DissolveBox_Left"));
	DissolveBox_Left->SetupAttachment(CraftingTableWidget);
	DissolveBox_Left->SetVisibility(false);
	DissolveBox_Right = CreateOptionalDefaultSubobject<UChildActorComponent>(TEXT("DissolveBox_Right"));
	DissolveBox_Right->SetupAttachment(CraftingTableWidget);
	DissolveBox_Right->SetVisibility(false);
}

// Called when the game starts or when spawned
void AAlbertosPawn::BeginPlay()
{
	Super::BeginPlay();

	OriginalMoveSpeed = AlbertosFloatingMovement->GetMaxSpeed();
	AlbertosAI = Cast<AAlbertosAIController>(GetController());

	UCraftingAlbertosWidget* CraftingWidget = Cast<UCraftingAlbertosWidget>(GetCraftingTableWidget());
	if (IsValid(CraftingWidget)) 
		CraftingWidget->SetAlbertosPawn(this);

	SetUpRandomSoundTimer();
}
// Called every frame
void AAlbertosPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAlbertosPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

#pragma region //////////////////// Inventory /////////////////
void AAlbertosPawn::TakeItem(AMarineCharacter* Character)
{
	if (IsValid(Character) == false)
		return;

	if (CraftingWidgetAnimationComponent->GetIsCraftingWidgetAnimationPlaying() == true)
		return;

	ToggleInventoryVisibility();

	if (CraftingTableWidget->IsVisible())
	{
		PlayerIsNearAlbertosComponent->SetRotateAlbertosTowardPlayer(true);
		Character->UpdateAlbertosInventory(true, true);
	}
}

void AAlbertosPawn::ItemHover(AMarineCharacter* Character)
{
	if (IsValid(OnAlbertosHoverMaterial) == false|| bIsHovered == true || CraftingWidgetAnimationComponent->GetIsCraftingWidgetAnimationPlaying() == true)
		return;

	AlbertosSkeletalMesh->SetMaterial(AlbertosHoverMaterialIndex, OnAlbertosHoverMaterial);

	if (HoverSound) 
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HoverSound, GetActorLocation());

	bIsHovered = true;
}

void AAlbertosPawn::ItemUnHover(AMarineCharacter* Character)
{
	if (IsValid(OnAlbertosUnHoverMaterial) == false || bIsHovered == false)
		return;

	AlbertosSkeletalMesh->SetMaterial(AlbertosHoverMaterialIndex, OnAlbertosUnHoverMaterial);
	bIsHovered = false;
}

void AAlbertosPawn::ToggleInventoryVisibility()
{
	if (CraftingTableWidget->IsVisible() == false)
	{
		if (AppearCraftingSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AppearCraftingSound, CraftingTableWidget->GetComponentLocation());

		ToggleVisibilityCraftingWidget();
		CraftingWidgetAnimationComponent->PrepareCraftingWidgetAnimation(true);
	}
	else
	{
		if (AppearCraftingSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AppearCraftingSound, CraftingTableWidget->GetComponentLocation());

		CraftingWidgetAnimationComponent->PrepareCraftingWidgetAnimation(false);
	}
}

void AAlbertosPawn::ToggleVisibilityCraftingWidget()
{
	CraftingTableWidget->ToggleVisibility();
	HologramMeshEffect->ToggleVisibility();
}
#pragma endregion

void AAlbertosPawn::EnableCraftingAnimation(bool bEnableCraftingAnim)
{
	if (bEnableCraftingAnim)
	{
		if (AlbertosCraftingAnimation)
			AlbertosSkeletalMesh->GetAnimInstance()->Montage_Play(AlbertosCraftingAnimation);
	}
	else
		AlbertosSkeletalMesh->GetAnimInstance()->Montage_Stop(CraftingAnimationBlendOut, AlbertosCraftingAnimation);

}

#pragma region /////// player action //////////
void AAlbertosPawn::CallAlbertosToThePlayer(FVector PlayerLoc)
{
	if (IsValid(AlbertosAI) == false) 
		return;

	if (TeleportAlbertosToPlayer(PlayerLoc) == true)
		return;

	PlayerLoc.Z = GetActorLocation().Z;
	AlbertosAI->CallAlbertosToThePlayer(PlayerLoc);
	PlayerIsNearAlbertosComponent->SetRotateAlbertosTowardPlayer(true);

	if (CallAlbertosSound) 
		UGameplayStatics::PlaySound2D(GetWorld(), CallAlbertosSound);

	ChangeMaxSpeedOfFloatingMovement(true);
}

// if player is further away from TeleportToPlayerRadius then teleport albertos to location near player
bool AAlbertosPawn::TeleportAlbertosToPlayer(FVector& PlayerLoc)
{
	if (FVector::Distance(PlayerLoc, GetActorLocation()) < MaxDistanceToPlayer)
		return false;

	FNavLocation RandomTeleportLocation;
	bool bFoundLoc = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(PlayerLoc, TeleportToPlayerRadius, RandomTeleportLocation);
	if (bFoundLoc == false)
		return false;

	SetActorLocation(RandomTeleportLocation.Location);
	return true;
}

void AAlbertosPawn::ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer)
{
	if (bTowardsPlayer == true) AlbertosFloatingMovement->MaxSpeed = MaxSpeedWhenMovingTowardsPlayer;
	else AlbertosFloatingMovement->MaxSpeed = OriginalMoveSpeed;
}

#pragma endregion

#pragma region /////////// SAVE/LOAD /////////////
void AAlbertosPawn::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (IsValid(SavedDataObject) == false)
		return;

	CurrentUniqueID = IDkey;
	FCustomDataSaved LocationData = SavedCustomData;
	LocationData.ObjectTransform = FTransform(GetActorRotation(), GetActorLocation());
	SavedDataObject->RemoveCustomSaveData(CurrentUniqueID);
	SavedDataObject->AddCustomSaveData(CurrentUniqueID, LocationData);
}

void AAlbertosPawn::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	SetActorLocation(SavedCustomData.ObjectTransform.GetLocation());
	SetActorRotation(SavedCustomData.ObjectTransform.GetRotation());
}

void AAlbertosPawn::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	LoadData(IDkey, SavedCustomData);
}

#pragma endregion 

#pragma region //////////// random sounds //////////
void AAlbertosPawn::SetUpRandomSoundTimer()
{
	const float& RandomSoundDuration = FMath::FRandRange(TimeRangeToPlayRandomSounds.GetLowerBoundValue(), TimeRangeToPlayRandomSounds.GetUpperBoundValue());
	GetWorld()->GetTimerManager().SetTimer(RandomSoundHandle, this, &AAlbertosPawn::PlayRandomAlbertoSound, RandomSoundDuration, false);
}

void AAlbertosPawn::PlayRandomAlbertoSound()
{
	if (RandomAlbertoSounds) 
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), RandomAlbertoSounds, GetActorLocation());

	SetUpRandomSoundTimer();
}
#pragma endregion

#pragma region //////////// GETTERS ////////////
class UUserWidget* AAlbertosPawn::GetCraftingTableWidget() const
{
	return CraftingTableWidget->GetUserWidgetObject();
}

void AAlbertosPawn::CallToggleOpenDoor(bool bOpenDoor) const
{
	PlayerIsNearAlbertosComponent->OpenAlbertosDoor(bOpenDoor);
}

bool AAlbertosPawn::GetIsCraftingTableWidgetVisible() const
{
	return CraftingTableWidget->IsVisible();
}
#pragma endregion