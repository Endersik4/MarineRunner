// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/ChangeMusicActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Objects/SavedDataObject.h"

// Sets default values
AChangeMusicActor::AChangeMusicActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ChangeMusicBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Change Music Box Comp"));
	RootComponent = ChangeMusicBoxComp;
	ChangeMusicBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChangeMusicBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AChangeMusicActor::BeginPlay()
{
	Super::BeginPlay();

	ChangeMusicBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AChangeMusicActor::ChangeMusicSoundBoxBeginOverlap);
	OriginalChangeMusic = bChangeMusic;
}

void AChangeMusicActor::ChangeMusicSoundBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bChangeMusic)
		ChangeBackgroundMusic();
	if (bPlaySound && SoundToPlay)
		UGameplayStatics::PlaySound2D(GetWorld(), SoundToPlay);

	DisableChangeMusic();

	ChangedMusicSaveData();
}

void AChangeMusicActor::ChangeBackgroundMusic()
{
	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance) || !IsValid(MusicToChange))
		return;

	GameInstance->SetCurrentExplorationMusic(MusicToChange);
	GameInstance->ChangeBackgroundMusic(EMusicType::EMT_Exploration);
}

void AChangeMusicActor::DisableChangeMusic(bool bDisable)
{
	SetActorTickEnabled(!bDisable);
	SetActorHiddenInGame(bDisable);
	SetActorEnableCollision(!bDisable);
}

void AChangeMusicActor::ChangedMusicSaveData()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void AChangeMusicActor::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1)
	{
		DisableChangeMusic();
	}
}

void AChangeMusicActor::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AChangeMusicActor::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	bChangeMusic = OriginalChangeMusic;
	DisableChangeMusic(false);
}
