// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Albertos/Widgets/Crafting/CraftingAlbertosWidget.h"
#include "MarineRunner/Albertos/Components/CraftItemAlbertosComponent.h"
#include "MarineRunner/Albertos/Components/CraftingWidgetAnimationComponent.h"
#include "MarineRunner/Albertos/Components/PlayerIsNearAlbertosComponent.h"
#include "MarineRunner/Albertos/Components/AlbertosToPlayerComponent.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

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
	AlbertosToPlayerComponent = CreateDefaultSubobject<UAlbertosToPlayerComponent>(TEXT("Call Albertos To Player Component"));

	CraftingTableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CraftingTableWidget"));
	CraftingTableWidget->SetupAttachment(AlbertosSkeletalMesh);
	CraftingTableWidget->SetDrawAtDesiredSize(true);

	HologramMeshEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HologramMeshEffect"));
	HologramMeshEffect->SetCollisionProfileName(FName(TEXT("NoCollision")));
	HologramMeshEffect->SetupAttachment(AlbertosSkeletalMesh);
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

	SetAlbertosPawnInCraftingWidget();

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
	if (!IsValid(Character))
		return;

	if (CraftingWidgetAnimationComponent->GetIsCraftingWidgetAnimationPlaying())
		return;

	ToggleInventoryVisibility();

	if (!CraftingTableWidget->IsVisible())
		return;

	PlayerIsNearAlbertosComponent->SetRotateAlbertosTowardPlayer(true);
	Character->UpdateAlbertosInventory(true, true);
}

void AAlbertosPawn::ItemHover(AMarineCharacter* Character)
{
	if (!IsValid(AlbertosSkeletalMesh) || !IsValid(OnAlbertosHoverMaterial))
		return;

	if (bIsHovered || CraftingWidgetAnimationComponent->GetIsCraftingWidgetAnimationPlaying())
		return;

	AlbertosSkeletalMesh->SetMaterial(AlbertosHoverMaterialIndex, OnAlbertosHoverMaterial);

	if (IsValid(HoverSound))
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HoverSound, GetActorLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("Hover Sound in AlbertosPawn is nullptr!"));

	bIsHovered = true;
}

void AAlbertosPawn::ItemUnHover(AMarineCharacter* Character)
{
	if (!IsValid(AlbertosSkeletalMesh) || !IsValid(OnAlbertosUnHoverMaterial))
		return;

	if (!bIsHovered)
		return;

	AlbertosSkeletalMesh->SetMaterial(AlbertosHoverMaterialIndex, OnAlbertosUnHoverMaterial);
	bIsHovered = false;
}

void AAlbertosPawn::ToggleInventoryVisibility()
{
	if (!IsValid(CraftingTableWidget))
		return;

	if (IsValid(AppearCraftingSound))
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AppearCraftingSound, CraftingTableWidget->GetComponentLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("Appear Crafting Sound in AlbertosPawn is nullptr!"));

	if (!CraftingTableWidget->IsVisible())
	{
		ToggleVisibilityCraftingWidget();
		CraftingWidgetAnimationComponent->PrepareCraftingWidgetAnimation(true);
	}
	else
	{
		CraftingWidgetAnimationComponent->PrepareCraftingWidgetAnimation(false);
	}
}

void AAlbertosPawn::ToggleVisibilityCraftingWidget()
{
	if (!IsValid(CraftingTableWidget) || !IsValid(HologramMeshEffect))
		return;

	CraftingTableWidget->ToggleVisibility();
	HologramMeshEffect->ToggleVisibility();
}
#pragma endregion

void AAlbertosPawn::EnableCraftingAnimation(bool bEnableCraftingAnim)
{
	if (!IsValid(AlbertosSkeletalMesh))
		return;

	if (bEnableCraftingAnim)
	{
		if (IsValid(AlbertosCraftingAnimation))
			AlbertosSkeletalMesh->GetAnimInstance()->Montage_Play(AlbertosCraftingAnimation);
		else 
			UE_LOG(LogTemp, Warning, TEXT("Montage Play - Albertos Crafting Animation in AlbertosPawn is nullptr!"));
	}
	else
	{
		if (IsValid(AlbertosCraftingAnimation))
			AlbertosSkeletalMesh->GetAnimInstance()->Montage_Stop(CraftingAnimationBlendOut, AlbertosCraftingAnimation);
		else
			UE_LOG(LogTemp, Warning, TEXT("Montage Stop - Albertos Crafting Animation in AlbertosPawn is nullptr!"));
	}

}

#pragma region /////////// SAVE/LOAD /////////////
void AAlbertosPawn::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (!IsValid(SavedDataObject))
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
	if (IsValid(RandomAlbertoSounds)) 
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), RandomAlbertoSounds, GetActorLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("Random Alberto Sounds in AlbertosPawn is nullptr!"));

	SetUpRandomSoundTimer();
}
#pragma endregion

#pragma region //////////// GETTERS ////////////
TObjectPtr<UUserWidget> AAlbertosPawn::GetCraftingTableWidget() const
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

float AAlbertosPawn::GetFloatingMovementMaxSpeed() const
{
	return  AlbertosFloatingMovement->GetMaxSpeed();
}
#pragma endregion

void AAlbertosPawn::SetFloatingMovementMaxSpeed(const float& NewSpeed)
{
	AlbertosFloatingMovement->MaxSpeed = NewSpeed;
}

void AAlbertosPawn::SetAlbertosPawnInCraftingWidget()
{
	if (!IsValid(GetCraftingTableWidget()))
		return;

	TObjectPtr<UCraftingAlbertosWidget> CraftingWidget = Cast<UCraftingAlbertosWidget>(GetCraftingTableWidget());
	if (!IsValid(CraftingWidget))
		return;

	CraftingWidget->SetAlbertosPawn(this);
}