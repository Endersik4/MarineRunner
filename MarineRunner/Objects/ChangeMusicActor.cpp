// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/ChangeMusicActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

// Sets default values
AChangeMusicActor::AChangeMusicActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChangeMusicBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Change Music Box Comp"));
	RootComponent = ChangeMusicBoxComp;
	ChangeMusicBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChangeMusicBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AChangeMusicActor::BeginPlay()
{
	Super::BeginPlay();

	ChangeMusicBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AChangeMusicActor::ChangeMusicBoxBeginOverlap);

}

// Called every frame
void AChangeMusicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChangeMusicActor::ChangeMusicBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bChangeMusic == true)
		ChangeBackgroundMusic();
	if (bPlaySound && SoundToPlay)
		UGameplayStatics::PlaySound2D(GetWorld(), SoundToPlay);

	DisableChangeMusic();
}

void AChangeMusicActor::ChangeBackgroundMusic()
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(GetGameInstance());
	if (IsValid(GameInstance) == false)
		return;

	GameInstance->SetCurrentExplorationMusic(MusicToChange);
	GameInstance->ChangeBackgroundMusic(EMusicType::EMT_Exploration);
}

void AChangeMusicActor::DisableChangeMusic()
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}