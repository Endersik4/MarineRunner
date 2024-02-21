// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

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

	OpenInventoryBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Inventory Box Component"));
	OpenInventoryBoxComponent->SetupAttachment(AlbertosSkeletalMesh);
	OpenInventoryBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OpenInventoryBoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

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

	AlbertosAI = Cast<AAlbertosAIController>(GetController());

	OriginalMoveSpeed = AlbertosFloatingMovement->GetMaxSpeed();

	UCraftingAlbertosWidget* CraftingWidget = Cast<UCraftingAlbertosWidget>(GetCraftingTableWidget());
	if (IsValid(CraftingWidget)) 
		CraftingWidget->SetAlbertosPawn(this);

	TimeForRandomSound = FMath::FRandRange(TimeRangeToPlayRandomSounds.GetLowerBoundValue(), TimeRangeToPlayRandomSounds.GetUpperBoundValue());
	GetWorld()->GetTimerManager().SetTimer(RandomSoundHandle, this, &AAlbertosPawn::PlayRandomAlbertoSound, TimeForRandomSound, false);

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

	if (CraftingWidgetAnimationComponent->GetIsCraftingWidgetAnimatinPlaying() == true)
		return;

	SetInventoryVisibility(!CraftingTableWidget->IsVisible());
	if (CraftingTableWidget->IsVisible())
	{
		Character->UpdateAlbertosInventory(true, true);
	}

	if (IsValid(SpawnedRandomSound))
	{
		if (SpawnedRandomSound->IsActive() == true)
			SpawnedRandomSound->Stop();
	}
}

void AAlbertosPawn::ItemHover(AMarineCharacter* Character)
{
	if (IsValid(OnAlbertosHoverMaterial) == false|| bIsHovered == true || CraftingWidgetAnimationComponent->GetIsCraftingWidgetAnimatinPlaying() == true)
		return;

	AlbertosSkeletalMesh->SetMaterial(3, OnAlbertosHoverMaterial);

	if (HoverSound) 
		UGameplayStatics::SpawnSoundAttached(HoverSound, AlbertosSkeletalMesh, FName(TEXT("Root")));
	bIsHovered = true;
}

void AAlbertosPawn::ItemUnHover(AMarineCharacter* Character)
{
	if (OnAlbertosUnHoverMaterial == nullptr || bIsHovered == false) 
		return;

	AlbertosSkeletalMesh->SetMaterial(3, OnAlbertosUnHoverMaterial);
	bIsHovered = false;
}

void AAlbertosPawn::SetInventoryVisibility(bool bVisible)
{
	if (bVisible)
	{
		if (CraftingTableWidget->IsVisible() == true)
			return;

		if (AppearCraftingSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AppearCraftingSound, CraftingTableWidget->GetComponentLocation());

		ToggleVisibilityCraftingWidget();
		CraftingWidgetAnimationComponent->PrepareCraftingWidgetAnimation(true);
	}
	else if (CraftingTableWidget->IsVisible() == true)
	{
		if (AppearCraftingSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AppearCraftingSound, CraftingTableWidget->GetComponentLocation());
		CraftingWidgetAnimationComponent->PrepareCraftingWidgetAnimation(false);
	}
}

void AAlbertosPawn::ToggleVisibilityCraftingWidget()
{
	CraftingTableWidget->ToggleVisibility();
	Hologram_1->ToggleVisibility();
	Hologram_2->ToggleVisibility();
}
#pragma endregion;

#pragma region /////// player action //////////
void AAlbertosPawn::CallAlbertoToThePlayer(FVector PlayerLoc)
{
	if (IsValid(AlbertosAI) == false) 
		return;

	if (TeleportAlbertosToPlayer(PlayerLoc) == true)
		return;

	PlayerLoc.Z = GetActorLocation().Z;
	AlbertosAI->CallAlbertosToThePlayer(PlayerLoc);

	if (CallAlbertosSound) 
		UGameplayStatics::PlaySound2D(GetWorld(), CallAlbertosSound);

	ChangeMaxSpeedOfFloatingMovement();
}

// if player is furhter away from TeleportToPlayerRadius then teleport albertos to location near player
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

void AAlbertosPawn::ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer)
{
	if (bTowardsPlayer == true) AlbertosFloatingMovement->MaxSpeed = MaxSpeedWhenMovingTowardsPlayer;
	else AlbertosFloatingMovement->MaxSpeed = OriginalMoveSpeed;
}

void AAlbertosPawn::PlayRandomAlbertoSound()
{
	if (RandomAlbertoSounds && CraftingTableWidget->IsVisible() == false) 
		SpawnedRandomSound = UGameplayStatics::SpawnSoundAttached(RandomAlbertoSounds, AlbertosSkeletalMesh, FName(TEXT("Root")));

	TimeForRandomSound = FMath::FRandRange(TimeRangeToPlayRandomSounds.GetLowerBoundValue(), TimeRangeToPlayRandomSounds.GetUpperBoundValue());
	GetWorld()->GetTimerManager().SetTimer(RandomSoundHandle, this, &AAlbertosPawn::PlayRandomAlbertoSound, TimeForRandomSound, false);
}

class UUserWidget* AAlbertosPawn::GetCraftingTableWidget() const
{
	return CraftingTableWidget->GetUserWidgetObject();
}

void AAlbertosPawn::CallToggleOpenDoor(bool bOpenDoor) const
{
	PlayerIsNearAlbertosComponent->ToggleOpenDoor(bOpenDoor);
}
