// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Interactive/PickableItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/MessageHandlerComponent.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"
#include "MarineRunner/Weapon/WeaponBase.h"
#include "MarineRunner/Objects/Components/SoundOnHitComponent.h"

APickupItem::APickupItem()
{
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
	if (!IsValid(Player))
		return;

	bool bItemWasAddedToInventory = GenerateItemToInventory(Player);
	if (!bItemWasAddedToInventory)
		return;

	if (IsValid(Player->GetHudWidget()))
	{ 
		Player->GetHudWidget()->PlayAppearAnimForItemHover(false);
	}
	
	if (IsValid(PickUpSound))
		UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Pick up sound is nullptr in PickableItem!"));
	
	SaveItemWasTaken();

	DisableItem();
}

bool APickupItem::GenerateItemToInventory(AMarineCharacter* Player)
{
	TObjectPtr<UInventoryComponent> Inventory = Player->GetInventoryComponent();
	if (!IsValid(Inventory))
		return false;

	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (!ItemInformationFromDataTable)
		return false;

	const bool bItemWasAddedToInventory = Inventory->AddItemToInventory(ItemRowName, ItemInformationFromDataTable->Item_Amount * AmountMultiplier);
	if (!bItemWasAddedToInventory)
		return false;

	SpawnWeaponForPlayer(Player, ItemInformationFromDataTable);
	return true;
}

void APickupItem::SpawnWeaponForPlayer(TObjectPtr<class AMarineCharacter> Player, FItemStruct* ItemDataFromDataTable)
{
	if (!ItemDataFromDataTable->bIsItWeapon || !ItemDataFromDataTable->WeaponClass.TryLoadClass<UObject>())
		return;

	const FTransform& WeaponTransform = FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f));
	TObjectPtr<AWeaponBase> SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(ItemDataFromDataTable->WeaponClass.TryLoadClass<UObject>(), WeaponTransform);
	if (!IsValid(SpawnedWeapon))
		return;

	SpawnedWeapon->PickUpWeaponItem(Player, bWasOnceTaken, CurrentMagazineCapacity);
}
#pragma endregion

#pragma region ///////// ITEM HOVERED/UNHOVERED //////////
void APickupItem::ItemHover(AMarineCharacter* Player)
{
	if (!IsValid(Player))
		return;

	FItemStruct* ItemInformationFromDataTable = Player->GetInventoryComponent()->GetItemInformationFromDataTable(ItemRowName);
	if (!ItemInformationFromDataTable)
		return;

	ItemMesh->SetRenderCustomDepth(true);

	if (!IsValid(Player->GetHudWidget()))
		return;

	Player->GetHudWidget()->SetItemHoverInformations(ItemInformationFromDataTable->Item_Name, ItemInformationFromDataTable->Item_Description, ItemInformationFromDataTable->Item_StorageIcon);
}

void APickupItem::ItemUnHover(AMarineCharacter* Player)
{
	if (!IsValid(Player))
		return;

	ItemMesh->SetRenderCustomDepth(false);

	if (!IsValid(Player->GetHudWidget()))
		return;

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
		const float& NewDissolveValue = FMath::Lerp(DissolveStartValue, DissolveEndValue, DissolveTimeElapsed / TimeToCraftAnItem);

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
	if (!IsValid(SavedDataObject))
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