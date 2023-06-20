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
	ItemMesh->SetCollisionProfileName(TEXT("GunCollision"));
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

}

void APickupItem::TakeItem(AMarineCharacter* Character, bool& bIsItWeapon)
{
	bIsItWeapon = false;
	UInventoryComponent* Inventory = Character->GetInventoryComponent();
	if (!Inventory) return;

	FItemStruct NewItem(Item_Name, Amount);

	FItemStruct* ItemFromInventory = Inventory->Inventory_Items.Find(Item_Name);

	//If there is an item with the same name, add the amount
	if (ItemFromInventory) 
	{ 
		ItemFromInventory->Item_Amount += Amount; 
	}
	else //if there are no items with the same name, add that item to the inventory
	{ 
		Inventory->Inventory_Items.Add(Item_Name, NewItem); 
	}

	//Update HUD to display particular item on hud
	Character->UpdateHudWidget();

	if (PickUpSound) UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
	Destroy();
}

