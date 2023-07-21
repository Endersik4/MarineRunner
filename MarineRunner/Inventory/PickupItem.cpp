// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Inventory/PickupItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/GunClasses/Gun.h"

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
	ItemMesh->bRenderCustomDepth = true; //Its for outline material 
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

void APickupItem::TakeItem(AMarineCharacter* Character, bool& bIsItWeapon)
{
	bIsItWeapon = false;
	UInventoryComponent* Inventory = Character->GetInventoryComponent();
	if (!Inventory) return;

	FItemStruct* ItemFromInventory = Inventory->Inventory_Items.Find(ItemSettings.Item_Name);

	//If there is an item with the same name, add the amount
	if (ItemFromInventory)
	{ 
		ItemFromInventory->Item_Amount += ItemSettings.Item_Amount; 
		if (ItemFromInventory->Item_Amount > 999) ItemFromInventory->Item_Amount -= ItemFromInventory->Item_Amount - 999;
	}
	else //if there are no items with the same name, add that item to the inventory
	{ 
		if (Inventory->Inventory_Items.Num() > 31) return;
		Inventory->Inventory_Items.Add(ItemSettings.Item_Name, ItemSettings);
	}

	//Update HUD to display particular item on hud
	Character->UpdateHudWidget();
	
	UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
	if (ItemSettings.bIsItWeapon == true) return;

	if (ItemSettings.bIsItCraftable == true) Character->UpdateAlbertosInventory(true, true);
	else Character->UpdateAlbertosInventory();
	Destroy();
}

void APickupItem::SetOverlayMaterial(UMaterialInstance* NewMaterial)
{
	if (NewMaterial == nullptr) return;

	DissolveDynamicMaterial = UMaterialInstanceDynamic::Create(NewMaterial, this);
	ItemMesh->SetOverlayMaterial(DissolveDynamicMaterial);
	
	bShouldDissolve = true;
}

void APickupItem::Dissolve(float Delta)
{
	if (bShouldDissolve == false || DissolveDynamicMaterial == nullptr) return;

	if (DissolveTimeElapsed <= ItemSettings.Item_TimeCraft)
	{
		float NewDissolveValue = FMath::Lerp(0.6f, -0.4f, DissolveTimeElapsed / ItemSettings.Item_TimeCraft);

		DissolveDynamicMaterial->SetScalarParameterValue(TEXT("Dissolve"), NewDissolveValue);
		DissolveTimeElapsed += Delta;
	}
	else
	{
		bShouldDissolve = false;
		ItemMesh->SetOverlayMaterial(nullptr);
	}
}


