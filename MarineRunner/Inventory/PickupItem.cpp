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
	if (!Inventory) 
		return;

	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (ItemInformationFromDataTable == nullptr)
		return;

	bool bAmountWasAdded = AddAmountToItemIfFound(Inventory->GetItemFromInventory(ItemRowName), ItemInformationFromDataTable->Item_Amount);

	if (bAmountWasAdded == false)
	{
		if (Inventory->Inventory_Items.Num() > 31) 
			return;

		Inventory->Inventory_Items.Add(*ItemInformationFromDataTable);
	}

	Player->UpdateHudWidget();
	Player->GetHudWidget()->PlayAppearAnimForItemHover(false);
	
	UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);

	AddCraftRecipeIfCraftable(Player, ItemInformationFromDataTable);

	SpawnWeaponForPlayer(Player, ItemInformationFromDataTable);

	Player->UpdateAlbertosInventory(true, ItemInformationFromDataTable->bIsItCraftable);
	
	SaveItemWasTaken();

	Destroy();
}

bool APickupItem::AddAmountToItemIfFound(FItemStruct* ItemFromInventory, float AmountToAdd)
{
	if (ItemFromInventory == nullptr)
		return false;

	if (ItemFromInventory->MaxItem_Amount != 0 && ItemFromInventory->Item_Amount >= ItemFromInventory->MaxItem_Amount)
		return false;

	ItemFromInventory->Item_Amount += AmountToAdd;

	return true;
}

void APickupItem::AddCraftRecipeIfCraftable(class AMarineCharacter* Player, FItemStruct* ItemDataFromDataTable)
{
	if (ItemDataFromDataTable->bIsItCraftable == false)
		return;

	// if there is craft recipe of this item then dont add another one
	if (Player->GetInventoryComponent()->Items_Recipes.FindByKey(*ItemDataFromDataTable) != nullptr)
		return;

	Player->GetMessageHandlerComponent()->SpawnNewRecipeUnlockedWidget();
	Player->GetInventoryComponent()->Items_Recipes.Add(*ItemDataFromDataTable);
}

void APickupItem::SpawnWeaponForPlayer(class AMarineCharacter* Player, FItemStruct* ItemDataFromDataTable)
{
	if (ItemDataFromDataTable->bIsItWeapon == false || ItemDataFromDataTable->WeaponClass == nullptr)
		return;

	AGun* SpawnedGun = GetWorld()->SpawnActorDeferred<AGun>(ItemDataFromDataTable->WeaponClass, FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
	if (IsValid(SpawnedGun) == false)
		return;

	SpawnedGun->AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemDataFromDataTable->WeaponSocketName);
	SpawnedGun->TakeGun(Player);
	SpawnedGun->FinishSpawning(FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
}

void APickupItem::ItemHover(AMarineCharacter* Player)
{
	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (ItemInformationFromDataTable == nullptr)
		return;

	if (IsValid(Player->GetHudWidget()) == false)
		return;

	ItemMesh->SetRenderCustomDepth(true);
	Player->GetHudWidget()->SetItemHoverInformations(ItemInformationFromDataTable->Item_Name, ItemInformationFromDataTable->Item_Description, ItemInformationFromDataTable->Item_StorageIcon);
}

void APickupItem::ItemUnHover(AMarineCharacter* Player)
{
	if (IsValid(Player->GetHudWidget()) == false)
		return;

	ItemMesh->SetRenderCustomDepth(false);
	Player->GetHudWidget()->PlayAppearAnimForItemHover(false);
}

void APickupItem::SetDissolveMaterial(class AMarineCharacter* Player, UMaterialInstance* NewMaterial, USkeletalMeshComponent* SkeletalMesh)
{
	if (NewMaterial == nullptr) 
		return;

	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (ItemInformationFromDataTable == nullptr)
		return;

	DissolveDynamicMaterial = UMaterialInstanceDynamic::Create(NewMaterial, this);
	if (SkeletalMesh == nullptr) ItemMesh->SetOverlayMaterial(DissolveDynamicMaterial);
	else
	{
		SkeletalMesh->SetOverlayMaterial(DissolveDynamicMaterial);
		WeaponSkeletalMesh = SkeletalMesh;
	}

	TimeToCraftAnItem = ItemInformationFromDataTable->Item_TimeCraft;
	bShouldDissolve = true;
}

void APickupItem::Dissolve(float Delta)
{
	if (bShouldDissolve == false || DissolveDynamicMaterial == nullptr) return;

	if (DissolveTimeElapsed <= TimeToCraftAnItem)
	{
		float NewDissolveValue = FMath::Lerp(DissolveStartValue, DissolveEndValue, DissolveTimeElapsed / TimeToCraftAnItem);

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

void APickupItem::ChangeSimulatingPhysics(bool bChange)
{
	ItemMesh->SetSimulatePhysics(bChange);
}

FItemStruct* APickupItem::GetItemDataFromDataTable()
{
	return Player->GetInventoryComponent()->Get;
}