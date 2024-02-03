// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Inventory/PickupItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Objects/ObjectsComponents/SoundOnHitComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "MarineRunner/Objects/SavedDataObject.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetCollisionProfileName(TEXT("ItemCollision"));
	ItemMesh->SetNotifyRigidBodyCollision(true);
	ItemMesh->SetGenerateOverlapEvents(false);
	ItemMesh->bRenderCustomDepth = false; //Its for outline material 

	SoundOnHitComponent = CreateDefaultSubobject<USoundOnHitComponent>(TEXT("Sound On Hit Comp"));
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Dissolve(DeltaTime);
}

void APickupItem::TakeItem(AMarineCharacter* Player)
{
	UInventoryComponent* Inventory = Player->GetInventoryComponent();
	if (!Inventory) return;

	FItemStruct* ItemFromInventory = Inventory->Inventory_Items.Find(ItemSettings.Item_Name);

	bool bAmountWasAdded = AddAmountToItemIfFound(ItemFromInventory);

	if (bAmountWasAdded == false)
	{
		if (Inventory->Inventory_Items.Num() > 31) 
			return;
		Inventory->Inventory_Items.Add(ItemSettings.Item_Name, ItemSettings);
	}

	Player->UpdateHudWidget();
	Player->GetHudWidget()->PlayAppearAnimForItemHover(false);
	
	UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);

	AddCraftRecipeIfCraftable(Player);

	SpawnWeaponForPlayer(Player);

	Player->UpdateAlbertosInventory(true, ItemSettings.bIsItCraftable);
	
	SaveItemWasTaken();

	Destroy();
}

bool APickupItem::AddAmountToItemIfFound(FItemStruct* ItemFromInventory)
{
	if (ItemFromInventory == nullptr)
		return false;

	if (ItemSettings.MaxItem_Amount != 0 && ItemFromInventory->Item_Amount >= ItemSettings.MaxItem_Amount)
		return false;

	ItemFromInventory->Item_Amount += ItemSettings.Item_Amount;

	return true;
}

void APickupItem::AddCraftRecipeIfCraftable(class AMarineCharacter* Player)
{
	if (ItemSettings.bIsItCraftable == false)
		return;

	// if there is craft recipe of this item then dont add another one
	if (Player->GetInventoryComponent()->Items_Recipes.FindByKey(GetItemSettings()) != nullptr)
		return;

	Player->GetMessageHandlerComponent()->SpawnNewRecipeUnlockedWidget();
	Player->GetInventoryComponent()->Items_Recipes.Add(GetItemSettings());
}

void APickupItem::SpawnWeaponForPlayer(class AMarineCharacter* Player)
{
	if (ItemSettings.bIsItWeapon == false || ItemSettings.WeaponClass == nullptr)
		return;

	AGun* SpawnedGun = GetWorld()->SpawnActorDeferred<AGun>(ItemSettings.WeaponClass, FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
	if (IsValid(SpawnedGun) == false)
		return;

	SpawnedGun->AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemSettings.WeaponSocketName);
	SpawnedGun->TakeGun(Player);
	SpawnedGun->FinishSpawning(FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
}

void APickupItem::ItemHover(UHUDWidget* MarineHUDWidget)
{
	ItemMesh->SetRenderCustomDepth(true);

	if (IsValid(MarineHUDWidget) == false) 
		return;

	MarineHUDWidget->SetItemHoverInformations(ItemSettings.Item_Name, ItemSettings.Item_Description, ItemSettings.Item_StorageIcon);
}

void APickupItem::ItemUnHover(UHUDWidget* MarineHUDWidget)
{
	ItemMesh->SetRenderCustomDepth(false);

	if (IsValid(MarineHUDWidget) == false) 
		return;

	MarineHUDWidget->PlayAppearAnimForItemHover(false);
}

void APickupItem::SetDissolveMaterial(UMaterialInstance* NewMaterial, USkeletalMeshComponent* SkeletalMesh)
{
	if (NewMaterial == nullptr) return;

	DissolveDynamicMaterial = UMaterialInstanceDynamic::Create(NewMaterial, this);
	if (SkeletalMesh == nullptr) ItemMesh->SetOverlayMaterial(DissolveDynamicMaterial);
	else
	{
		SkeletalMesh->SetOverlayMaterial(DissolveDynamicMaterial);
		WeaponSkeletalMesh = SkeletalMesh;
	}
	
	bShouldDissolve = true;
}

void APickupItem::Dissolve(float Delta)
{
	if (bShouldDissolve == false || DissolveDynamicMaterial == nullptr) return;

	if (DissolveTimeElapsed <= ItemSettings.Item_TimeCraft)
	{
		float NewDissolveValue = FMath::Lerp(DissolveStartValue, DissolveEndValue, DissolveTimeElapsed / ItemSettings.Item_TimeCraft);

		DissolveDynamicMaterial->SetScalarParameterValue(TEXT("Dissolve"), NewDissolveValue);
		DissolveTimeElapsed += Delta;
	}
	else
	{
		bShouldDissolve = false;

		if (WeaponSkeletalMesh == nullptr) ItemMesh->SetOverlayMaterial(nullptr);
		else WeaponSkeletalMesh->SetOverlayMaterial(nullptr);
	}
}

void APickupItem::SetCollisionNewResponse(ECollisionChannel ChannelName, ECollisionResponse NewResponse)
{
	ItemMesh->SetCollisionResponseToChannel(ChannelName, NewResponse);
}

void APickupItem::SaveItemWasTaken()
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	SavedDataObject->AddCustomSaveData(this, 1);
}

void APickupItem::LoadData(int32 StateOfData)
{
	if (StateOfData == 1)
	{
		Destroy();
	}
}

