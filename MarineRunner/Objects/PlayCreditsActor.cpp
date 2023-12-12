// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/PlayCreditsActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

// Sets default values
APlayCreditsActor::APlayCreditsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void APlayCreditsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayCreditsActor::CreditsBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCreditsActive == true)
		return;

	bCreditsActive = true;
	SpawnWidget();

	GetWorldTimerManager().SetTimer(LoadCreditsMapHandle, this, &APlayCreditsActor::LoadCreditsMap, MoveToCreditsLevelTime, false);
}

void APlayCreditsActor::SpawnWidget()
{
	UUserWidget* SpawnedWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), WidgetClassToSpawnWhenOverlap);
	if (IsValid(SpawnedWidget) == false)
		return;

	SpawnedWidget->AddToViewport();
}

void APlayCreditsActor::LoadCreditsMap()
{
	UGameplayStatics::OpenLevel(GetWorld(), CreditsLevelName);
}

