// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/SaveLoadPlayerComponent.h"
#include "MarineRunner/Objects/SavedDataObject.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckpointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointBox"));
	RootComponent = CheckpointBox;
	CheckpointBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CheckpointBox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	CheckpointBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnCheckpointBoxBeginOverlap);
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::OnCheckpointBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarineCharacter* MarinePawn = Cast<AMarineCharacter>(OtherActor);
	if (IsValid(MarinePawn) == false) 
		return;

	MarinePawn->GetSaveLoadPlayerComponent()->SaveGame(SaveToNameAfterCheckpoint, SaveNumberWildCard);

	SaveCheckpoint();
	DisableCheckpoint();
}

void ACheckpoint::SaveCheckpoint()
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void ACheckpoint::DisableCheckpoint()
{
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ACheckpoint::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState != 1)
		return;

	DisableCheckpoint();
}

void ACheckpoint::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

