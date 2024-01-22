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
	ItemMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
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

void APickupItem::TakeItem(AMarineCharacter* Character)
{
	UInventoryComponent* Inventory = Character->GetInventoryComponent();
	if (!Inventory) return;

	FItemStruct* ItemFromInventory = Inventory->Inventory_Items.Find(ItemSettings.Item_Name);

	//If there is an item with the same name, add the amount
	if (ItemFromInventory)
	{ 
		if (ItemSettings.MaxItem_Amount != 0 && ItemFromInventory->Item_Amount >= ItemSettings.MaxItem_Amount)
		{
			return;
		}
		ItemFromInventory->Item_Amount += ItemSettings.Item_Amount; 
	}
	else //if there are no items with the same name, add that item to the inventory
	{ 
		if (Inventory->Inventory_Items.Num() > 31) return;
		Inventory->Inventory_Items.Add(ItemSettings.Item_Name, ItemSettings);
	}

	Character->UpdateHudWidget();
	Character->GetHudWidget()->PlayAppearAnimForItemHover(false);
	
	UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);

	if (ItemSettings.bIsItWeapon == true && ItemSettings.WeaponClass != nullptr)
	{
		AGun* SpawnedGun = GetWorld()->SpawnActorDeferred<AGun>(ItemSettings.WeaponClass, FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
		if (IsValid(SpawnedGun))
		{
			SpawnedGun->AttachToComponent(Character->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("Weapon")));
			SpawnedGun->TakeGun(Character);
			SpawnedGun->FinishSpawning(FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
		}
	}

	if (ItemSettings.bIsItCraftable == true)
	{
		if (Character->GetInventoryComponent()->Items_Recipes.FindByKey(GetItemSettings()) == nullptr)
		{
			Character->GetMessageHandlerComponent()->SpawnNewRecipeUnlockedWidget();
			Character->GetInventoryComponent()->Items_Recipes.Add(GetItemSettings());
		}

		Character->UpdateAlbertosInventory(true, true);
	}
	else Character->UpdateAlbertosInventory();
	
	Destroy();
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

