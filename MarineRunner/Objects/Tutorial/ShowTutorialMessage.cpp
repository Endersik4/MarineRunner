// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Tutorial/ShowTutorialMessage.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Objects/Tutorial/TutorialMessageWidget.h"


// Sets default values
AShowTutorialMessage::AShowTutorialMessage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShowMessageBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Show Message Box Comp"));
	RootComponent = ShowMessageBoxComp;
	ShowMessageBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ShowMessageBoxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void AShowTutorialMessage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShowTutorialMessage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShowTutorialMessage::ShowMessageBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TutorialMessageWidgetClass == nullptr || bCanShowTutorialMessage == false)
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC) == false)
		return;

	if (PC->GetPawn() != OtherActor)
		return;
	
	UTutorialMessageWidget* TutorialMessageWidget = Cast<UTutorialMessageWidget>(CreateWidget(PC, TutorialMessageWidgetClass));
	TutorialMessageWidget->AddToViewport();

	bCanShowTutorialMessage = false;
}

