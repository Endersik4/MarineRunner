// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Inventory/PickupItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Objects/ObjectsComponents/SoundOnHitComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "MarineRunner/Objects/SavedDataObject.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"

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

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Dissolve(DeltaTime);
}

#pragma region ///////////// TAKE ITEM ////////////////
void APickupItem::TakeItem(AMarineCharacter* Player)
{
	UInventoryComponent* Inventory = Player->GetInventoryComponent();
	if (!Inventory) 
		return;

	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (ItemInformationFromDataTable == nullptr)
		return;

	if (ItemInformationFromDataTable->bIsItWeapon == true && Player->GetWeaponInventoryComponent()->CanPlayerTakeWeaponToInventory() == false)
	{
		Player->GetMessageHandlerComponent()->SpawnNotEnoughSlotsForWeaponWidget();
		return;
	}

	bool bAmountWasAdded = AddAmountToItemIfFound(Inventory->GetItemFromInventory(ItemRowName), ItemInformationFromDataTable->Item_Amount * AmountMultiplier);

	if (bAmountWasAdded == false)
	{
		if (Inventory->Inventory_Items.Num() > 31) 
			return;

		FItemStruct ItemToAdd = *ItemInformationFromDataTable;
		ItemToAdd.Item_Amount *= AmountMultiplier;
		Inventory->Inventory_Items.Add(ItemToAdd);
	}

	Player->UpdateHudWidget();
	Player->GetHudWidget()->PlayAppearAnimForItemHover(false);
	
	if (PickUpSound)	
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

	FTransform WeaponTransform = FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f));
	AGun* SpawnedGun = GetWorld()->SpawnActor<AGun>(ItemDataFromDataTable->WeaponClass, WeaponTransform);
	if (IsValid(SpawnedGun) == false)
		return;

	SpawnedGun->AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemDataFromDataTable->WeaponSocketName);
	SpawnedGun->TakeGun(Player, bWasOnceTaken, CurrentMagazineCapacity);
}
#pragma endregion

#pragma region ///////// ITEM HOVERED/UNHOVERED //////////
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
#pragma endregion

#pragma region /////////// DISSOLVE MATERIAL ON MESH //////////////
void APickupItem::SetDissolveMaterial(class AMarineCharacter* Player, float TimeToEndDisolve, UMaterialInstance* OverlayInstanceMaterial)
{
	if (IsValid(OverlayInstanceMaterial) == false) 
		return;

	DissolveDynamicMaterial = UMaterialInstanceDynamic::Create(OverlayInstanceMaterial, this);
	if (IsValid(DissolveDynamicMaterial) == false)
		return;

	ItemMesh->SetOverlayMaterial(DissolveDynamicMaterial);

	TimeToCraftAnItem = TimeToEndDisolve;
	bShouldDissolve = true;
}

void APickupItem::Dissolve(float Delta)
{
	if (bShouldDissolve == false || IsValid(DissolveDynamicMaterial) == false) 
		return;

	if (DissolveTimeElapsed <= TimeToCraftAnItem)
	{
		float NewDissolveValue = FMath::Lerp(DissolveStartValue, DissolveEndValue, DissolveTimeElapsed / TimeToCraftAnItem);

		DissolveDynamicMaterial->SetScalarParameterValue(DisolveScalarParameterName, NewDissolveValue);
		DissolveTimeElapsed += Delta;

		return;
	}

	bShouldDissolve = false;
	ItemMesh->SetOverlayMaterial(nullptr);
}
#pragma endregion 

#pragma region /////////// SAVE/LOAD /////////////
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
#pragma endregion