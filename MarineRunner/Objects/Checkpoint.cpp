// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Checkpoint.h"
#include "Components/BoxComponent.h"
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
	if (bDisabledCheckpoint == true)
		return;

	AMarineCharacter* MarinePawn = Cast<AMarineCharacter>(OtherActor);
	if (IsValid(MarinePawn) == false) 
		return;

	bDisabledCheckpoint = true;
	SaveCheckpoint();
	DisableCheckpoint();

	MarinePawn->GetSaveLoadPlayerComponent()->SaveGame(SaveToNameAfterCheckpoint, SaveNumberWildCard);
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

