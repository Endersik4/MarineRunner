// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Objects/ChestWithItems.h"
#include "Components/BoxComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Objects/Door/DoorWidgets/DoorPanelWidget.h"
#include "MarineRunner/Objects/SavedDataObject.h"


// Sets default values
AChestWithItems::AChestWithItems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	
	FrontChestPanelWidget = Cast<UDoorPanelWidget>(FrontPanelWidget->GetUserWidgetObject());
	if (FrontChestPanelWidget == nullptr) return;

	if (IsValid(FrontChestPanelWidget) == true)
	{
		FrontChestPanelWidget->SetDoorActor(this);
		FrontChestPanelWidget->SetCanCloseObject(false);

		if (bUsePinCode == true)
			FrontChestPanelWidget->ChangeDoorPanelToUsePin(PinCode);
	}
}

void AChestWithItems::ClickedOpenButton(class UDoorPanelWidget* ClickedWidget)
{
	if (bIsChestOpen == true) 
		return;

	OpenChest();
}

void AChestWithItems::PinCorrect()
{
	SaveChestState(1);
}

// Called every frame
void AChestWithItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChestWithItems::OpenChest()
{
	if (OpenChestSound) UGameplayStatics::SpawnSoundAtLocation(GetWorld(), OpenChestSound, ChestSkeletalMesh->GetSocketLocation(FName(TEXT("temSpawnLocation"))));
	ChestSkeletalMesh->SetMaterial(3, UpperOpenLockMaterial);
	ChestSkeletalMesh->PlayAnimation(OpenChestAnimation, false);

	for (const TPair<TSubclassOf<class APickupItem>, FItemRandomSpawnStruct> CurrentPair : ItemsToSpawn)
	{
		for (int i = 0; i != CurrentPair.Value.AmountItems; i++)
		{
			float ShouldSpawn = FMath::FRandRange(0.f, 100.f);
			if (ShouldSpawn > CurrentPair.Value.PercentOfSpawningItem) continue;

			FVector Location = ChestSkeletalMesh->GetSocketLocation(FName(TEXT("ItemSpawnLocation"))) + CurrentPair.Value.OffsetFromSpawnLocation;
			FRotator Rotation = GetActorRotation();

			APickupItem* SpawnedItem = GetWorld()->SpawnActorDeferred<APickupItem>(CurrentPair.Key, FTransform(Rotation,Location));
			if (IsValid(SpawnedItem) == false)
				continue;
			SpawnedItem->SaveItemIfSpawnedRunTime();
			SpawnedItem->FinishSpawning(FTransform(Rotation, Location));
		}
	}

	SaveChestState(2);
	bIsChestOpen = true;
}

void AChestWithItems::SaveChestState(int32 SaveState)
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
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
		FrontChestPanelWidget->SetVisibility(ESlateVisibility::Hidden);
		ChestSkeletalMesh->PlayAnimation(OpenChestAnimation, false);
		ChestSkeletalMesh->SetPosition(1.3f);
	}
}

void AChestWithItems::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
}
