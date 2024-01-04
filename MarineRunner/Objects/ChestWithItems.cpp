// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Objects/ChestWithItems.h"
#include "Components/BoxComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Objects/Door/DoorWidgets/DoorPanelWidget.h"


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
	if (bIsChestOpen == true) return;

	OpenChest();
}

void AChestWithItems::PinCorrect()
{
	;
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

	TArray<TSubclassOf<APickupItem>> ItemsArray;
	ItemsToSpawn.GenerateKeyArray(ItemsArray);
	for (TSubclassOf<APickupItem> SpawnedItem : ItemsArray)
	{
		FItemRandomSpawnStruct* ItemRandomStruct = ItemsToSpawn.Find(SpawnedItem);
		if (ItemRandomStruct == nullptr) continue;

		for (int i = 0; i != ItemRandomStruct->AmountItems; i++)
		{
			float ShouldSpawn = FMath::FRandRange(0.f, 100.f);
			if (ShouldSpawn > ItemRandomStruct->PercentOfSpawningItem) continue;

			FVector Location = ChestSkeletalMesh->GetSocketLocation(FName(TEXT("ItemSpawnLocation"))) + ItemRandomStruct->OffsetFromSpawnLocation;
			FRotator Rotation = GetActorRotation();

			GetWorld()->SpawnActor<APickupItem>(SpawnedItem, Location, Rotation);
		}
	}

	bIsChestOpen = true;
}
