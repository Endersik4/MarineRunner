// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/PlayCreditsActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

APlayCreditsActor::APlayCreditsActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CreditsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CreditsBox"));
	RootComponent = CreditsBox;
	CreditsBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CreditsBox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void APlayCreditsActor::BeginPlay()
{
	Super::BeginPlay();
	
	CreditsBox->OnComponentBeginOverlap.AddDynamic(this, &APlayCreditsActor::CreditsBoxBeginOverlap);
}

void APlayCreditsActor::CreditsBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCreditsActive)
		return;

	bCreditsActive = true;
	SpawnWidget();

	GetWorldTimerManager().SetTimer(LoadCreditsMapHandle, this, &APlayCreditsActor::LoadCreditsMap, MoveToCreditsLevelTime, false);
}

void APlayCreditsActor::SpawnWidget()
{
	TObjectPtr<UUserWidget> SpawnedWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), WidgetClassToSpawnWhenOverlap);
	if (!IsValid(SpawnedWidget))
		return;

	SpawnedWidget->AddToViewport();
}

void APlayCreditsActor::LoadCreditsMap()
{
	UGameplayStatics::OpenLevel(GetWorld(), CreditsLevelName);
}

