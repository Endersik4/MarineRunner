// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Interactive/ChestWithItems.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"
#include "MarineRunner/Objects/Interactive/PanelWithPin/PanelWithPinWidget.h"
#include "MarineRunner/Objects/Interactive/PickableItem.h"

AChestWithItems::AChestWithItems()
{
	PrimaryActorTick.bCanEverTick = false;

	ChestSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest Skeletal Mesh"));
	ChestSkeletalMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	ChestSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	ChestSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	RootComponent = ChestSkeletalMesh;

	FrontPanelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Front Panel Widget"));
	FrontPanelWidget->SetupAttachment(ChestSkeletalMesh);
	FrontPanelWidget->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void AChestWithItems::BeginPlay()
{
	Super::BeginPlay();
	
	SetUpFrontChestPanelWidget();
}

void AChestWithItems::SetUpFrontChestPanelWidget()
{
	if (!IsValid(FrontPanelWidget->GetUserWidgetObject()))
		return;

	FrontChestPanelWidget = Cast<UDoorPanelWidget>(FrontPanelWidget->GetUserWidgetObject());
	if (!IsValid(FrontChestPanelWidget))
		return;

	FrontChestPanelWidget->SetActorToUnlock(this);
	FrontChestPanelWidget->SetCanCloseObject(false);

	if (bUsePinCode)
		FrontChestPanelWidget->ChangeToUsePin(PinCode);
}

void AChestWithItems::ClickedOpenButton(class UDoorPanelWidget* ClickedWidget)
{
	if (bIsChestOpen) 
		return;

	FrontPanelWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OpenChest();
}

void AChestWithItems::PinCorrect()
{
	SaveChestState(1);
}

void AChestWithItems::OpenChest()
{
	if (IsValid(OpenChestSound))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), OpenChestSound, ChestSkeletalMesh->GetSocketLocation(SocketNameToSpawnItems));
	else
		UE_LOG(LogTemp, Warning, TEXT("Open Chest Sound is nullptr in ChestWithItems!"));

	ChestSkeletalMesh->SetMaterial(IndexToChangeOpenLockMaterial, UpperOpenLockMaterial);
	ChestSkeletalMesh->PlayAnimation(OpenChestAnimation, false);

	FTimerHandle test;
	GetWorld()->GetTimerManager().SetTimer(test, this, &AChestWithItems::SpawnItems, 0.1f, false);

	bIsChestOpen = true;
}

void AChestWithItems::SpawnItems()
{
	for (const TPair<TSubclassOf<class APickupItem>, FItemRandomSpawnStruct>& CurrentPair : ItemsToSpawn)
	{
		for (int i = 0; i != CurrentPair.Value.AmountItems; i++)
		{
			const float& ShouldSpawn = FMath::FRandRange(0.f, 100.f);
			if (ShouldSpawn > CurrentPair.Value.PercentOfSpawningItem)
				continue;

			const FVector& ItemLocation = ChestSkeletalMesh->GetSocketLocation(SocketNameToSpawnItems) + CurrentPair.Value.OffsetFromSpawnLocation;
			const FTransform& ItemTransform = FTransform(GetActorRotation(), ItemLocation);

			TObjectPtr<APickupItem> SpawnedItem = GetWorld()->SpawnActorDeferred<APickupItem>(CurrentPair.Key, ItemTransform);
			if (!IsValid(SpawnedItem))
				continue;

			SpawnedItem->SaveItemIfSpawnedRunTime();
			SpawnedItem->FinishSpawning(ItemTransform);
		}
	}

	SaveChestState(bUsePinCode ? 4 : 2);
}

void AChestWithItems::SaveChestState(int32 SaveState)
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, SaveState));
}

void AChestWithItems::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1)
	{
		FrontChestPanelWidget->PinIsCorrect(false);
	}
	else if (SavedCustomData.ObjectState == 2)
	{
		bIsChestOpen = true;
		ChestSkeletalMesh->PlayAnimation(OpenChestAnimation, false);
		ChestSkeletalMesh->SetPosition(OpenChestAnimation->GetPlayLength());
		ChestSkeletalMesh->SetMaterial(IndexToChangeOpenLockMaterial, UpperOpenLockMaterial);

		if (IsValid(FrontChestPanelWidget))
			FrontChestPanelWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AChestWithItems::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AChestWithItems::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{


	if (SavedCustomData.ObjectState == 2 || SavedCustomData.ObjectState == 4)
	{
		bIsChestOpen = false;

		if (IsValid(FrontChestPanelWidget))
		{
			FrontChestPanelWidget->SetVisibility(ESlateVisibility::Visible);
			FrontChestPanelWidget->RestartDoorPanelWidget();
		}

		FrontPanelWidget->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		ChestSkeletalMesh->PlayAnimation(OpenChestAnimation, false);
		ChestSkeletalMesh->SetPosition(0.f);
		ChestSkeletalMesh->Stop();
		ChestSkeletalMesh->SetMaterial(IndexToChangeOpenLockMaterial, UpperClosedLockMaterial);
	}

	if (SavedCustomData.ObjectState == 1 || SavedCustomData.ObjectState == 4)
	{
		if (bUsePinCode == true && IsValid(FrontChestPanelWidget))
			FrontChestPanelWidget->ChangeToUsePin(PinCode);
	}
}
