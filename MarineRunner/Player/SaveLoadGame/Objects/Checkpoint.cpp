// Copyright Adam Bartela.All Rights Reserved


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/SaveLoadGame/SaveLoadPlayerComponent.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

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

	DisableCheckpoint();
	EnableCheckpointAfterDelay();
}

void ACheckpoint::EnableCheckpointAfterDelay()
{
	FTimerHandle EnableCheckpointAfterDelayHandle;
	FTimerDelegate EnableCheckpointDelegate = FTimerDelegate::CreateUObject(this, &ACheckpoint::DisableCheckpoint, true);
	GetWorld()->GetTimerManager().SetTimer(EnableCheckpointAfterDelayHandle, EnableCheckpointDelegate, EnableCheckpointAtStartTime, false);
}

void ACheckpoint::OnCheckpointBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDisabledCheckpoint || !IsValid(OtherActor))
		return;

	TObjectPtr<AMarineCharacter> MarinePawn = Cast<AMarineCharacter>(OtherActor);
	if (!IsValid(MarinePawn)) 
		return;

	bDisabledCheckpoint = true;
	SaveCheckpointWasUsed();
	DisableCheckpoint();

	MarinePawn->CheckpointNumber = CheckpointNumber; //ztgk, delete later
	MarinePawn->GetSaveLoadPlayerComponent()->SaveGame(SaveToNameAfterCheckpoint, SaveNumberWildCard);
}

void ACheckpoint::SaveCheckpointWasUsed()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void ACheckpoint::DisableCheckpoint(bool bDisable)
{
	SetActorEnableCollision(bDisable);
	SetActorTickEnabled(bDisable);
}

void ACheckpoint::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState != 1)
		return;

	bDisabledCheckpoint = true;
	DisableCheckpoint();
}

void ACheckpoint::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void ACheckpoint::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

