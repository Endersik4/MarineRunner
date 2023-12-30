// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/AlbertosClasses/CallAlbertosActor.h"
#include "Components/BoxComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "AlbertosPawn.h"
// Sets default values
ACallAlbertosActor::ACallAlbertosActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CallAlbertosBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CallAlbertosBoxComp"));
	RootComponent = CallAlbertosBoxComp;
	CallAlbertosBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CallAlbertosBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called when the game starts or when spawned
void ACallAlbertosActor::BeginPlay()
{
	Super::BeginPlay();
	
	CallAlbertosBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACallAlbertosActor::CallAlbertosBoxBeginOverlap);
}

// Called every frame
void ACallAlbertosActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACallAlbertosActor::CallAlbertosBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bWasCalled == true)
		return;

	AMarineCharacter* Player = Cast<AMarineCharacter>(OtherActor);
	if (IsValid(Player) == false)
		return;
	if (IsValid(Player->AlbertoPawn) == false)
		return;

	Player->AlbertoPawn->SetActorLocation(TeleportAlbertosLocation);

	bWasCalled = true;
}
