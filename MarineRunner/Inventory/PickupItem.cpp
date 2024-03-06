// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Inventory/PickupItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/GunClasses/Components/GunControlsComponent.h"
#include "MarineRunner/Objects/ObjectsComponents/SoundOnHitComponent.h"
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

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

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
	TObjectPtr<UInventoryComponent> Inventory = Player->GetInventoryComponent();
	if (!IsValid(Inventory)) 
		return;

	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (!ItemInformationFromDataTable)
		return;

	if (ItemInformationFromDataTable->bIsItWeapon && !Player->GetWeaponInventoryComponent()->CanPlayerTakeWeaponToInventory())
	{
		Player->GetMessageHandlerComponent()->SpawnNotEnoughSlotsForWeaponWidget();
		return;
	}

	bool bAmountWasAdded = AddAmountToItemIfFound(Inventory->GetItemFromInventory(ItemRowName), ItemInformationFromDataTable->Item_Amount * AmountMultiplier);

	if (!bAmountWasAdded)
	{
		if (Inventory->Inventory_Items.Num() > Inventory->GetMaxSlotsInInventory())
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

	DisableItem();
}

bool APickupItem::AddAmountToItemIfFound(FItemStruct* ItemFromInventory, float AmountToAdd)
{
	if (!ItemFromInventory)
		return false;

	if (ItemFromInventory->MaxItem_Amount != 0 && ItemFromInventory->Item_Amount >= ItemFromInventory->MaxItem_Amount)
		return false;

	ItemFromInventory->Item_Amount += AmountToAdd;

	return true;
}

void APickupItem::AddCraftRecipeIfCraftable(TObjectPtr<class AMarineCharacter> Player, FItemStruct* ItemDataFromDataTable)
{
	if (!ItemDataFromDataTable->bIsItCraftable)
		return;

	// if there is craft recipe of this item then dont add another one
	if (Player->GetInventoryComponent()->Items_Recipes.FindByKey(*ItemDataFromDataTable))
		return;

	Player->GetMessageHandlerComponent()->SpawnNewRecipeUnlockedWidget();
	Player->GetInventoryComponent()->Items_Recipes.Add(*ItemDataFromDataTable);
}

void APickupItem::SpawnWeaponForPlayer(TObjectPtr<class AMarineCharacter> Player, FItemStruct* ItemDataFromDataTable)
{
	if (!ItemDataFromDataTable->bIsItWeapon || !ItemDataFromDataTable->WeaponClass)
		return;

	FTransform WeaponTransform = FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f));
	TObjectPtr<AGun> SpawnedGun = GetWorld()->SpawnActor<AGun>(ItemDataFromDataTable->WeaponClass, WeaponTransform);
	if (!IsValid(SpawnedGun))
		return;
	
	SpawnedGun->AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpawnedGun->GetGunControlsComponent()->GetAttachToSocketName());
	SpawnedGun->GetGunControlsComponent()->TakeGun(Player, bWasOnceTaken, CurrentMagazineCapacity);
}
#pragma endregion

#pragma region ///////// ITEM HOVERED/UNHOVERED //////////
void APickupItem::ItemHover(AMarineCharacter* Player)
{
	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (!ItemInformationFromDataTable)
		return;

	if (!IsValid(Player->GetHudWidget()))
		return;

	ItemMesh->SetRenderCustomDepth(true);
	Player->GetHudWidget()->SetItemHoverInformations(ItemInformationFromDataTable->Item_Name, ItemInformationFromDataTable->Item_Description, ItemInformationFromDataTable->Item_StorageIcon);
}

void APickupItem::ItemUnHover(AMarineCharacter* Player)
{
	if (!IsValid(Player->GetHudWidget()))
		return;

	ItemMesh->SetRenderCustomDepth(false);
	Player->GetHudWidget()->PlayAppearAnimForItemHover(false);
}
#pragma endregion

#pragma region /////////// DISSOLVE MATERIAL ON MESH //////////////
void APickupItem::SetDissolveMaterial(class AMarineCharacter* Player, float TimeToEndDisolve, UMaterialInstance* OverlayInstanceMaterial)
{
	if (!IsValid(OverlayInstanceMaterial)) 
		return;

	DissolveDynamicMaterial = UMaterialInstanceDynamic::Create(OverlayInstanceMaterial, this);
	if (!IsValid(DissolveDynamicMaterial))
		return;

	ItemMesh->SetOverlayMaterial(DissolveDynamicMaterial);

	TimeToCraftAnItem = TimeToEndDisolve;
	bShouldDissolve = true;
}

void APickupItem::Dissolve(float Delta)
{
	if (!bShouldDissolve || !IsValid(DissolveDynamicMaterial)) 
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
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));
	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID != 0)
	{
		SavedDataObject->RemoveCustomSaveData(CurrentUniqueID);
		return;
	}

	SaveItem(SavedDataObject, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1, CurrentMagazineCapacity));
}

void APickupItem::SaveItemIfSpawnedRunTime()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));
	if (!IsValid(SavedDataObject) == false)
		return;

	FCustomDataSaved ItemSpawnedData = FCustomDataSaved(ESavedDataState::ESDS_SpawnObject, GetClass(), FTransform(GetActorRotation(), GetActorLocation()), CurrentMagazineCapacity);
	ItemSpawnedData.ObjectState = 3;
	ItemSpawnedData.ObjectToSaveData = this;
	SaveItem(SavedDataObject, ItemSpawnedData);
}

void APickupItem::SaveItem(TObjectPtr<class ASavedDataObject> SavedDataObject, const FCustomDataSaved& DataToSave)
{
	CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();
	FCustomDataSaved SavedDataItemWasSpawned = DataToSave;
	SavedDataObject->AddCustomSaveData(CurrentUniqueID, SavedDataItemWasSpawned);
}

void APickupItem::SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	SavedDataObject->RemoveCustomSaveData(IDkey);
	FCustomDataSaved UpdatedData = SavedCustomData;
	UpdatedData.ObjectTransform = FTransform(GetActorRotation(), GetActorLocation());
	UpdatedData.ObjectToSaveData = this;
	SavedDataObject->AddCustomSaveData(IDkey, UpdatedData);
}

void APickupItem::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	// Load Data
	if (SavedCustomData.ObjectState == 3)
	{
		CurrentMagazineCapacity = SavedCustomData.ValueToSave;
		bWasOnceTaken = true;
		CurrentUniqueID = IDkey;
		return;
	}

	// "Destroy" Object
	if (SavedCustomData.ObjectState == 1)
	{
		DisableItem();
	}
}

void APickupItem::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (SavedCustomData.ObjectState == 1)
	{
		CurrentUniqueID = 0;
		DisableItem(false);
	}
	else if (SavedCustomData.ObjectState == 3)
	{
		SavedDataObject->RemoveCustomSaveData(IDkey);
		Destroy();
	}
}

void APickupItem::DisableItem(bool bDisable)
{
	SetActorTickEnabled(!bDisable);
	SetActorHiddenInGame(bDisable);
	SetActorEnableCollision(!bDisable);
	ItemMesh->SetSimulatePhysics(!bDisable);
}
#pragma endregion