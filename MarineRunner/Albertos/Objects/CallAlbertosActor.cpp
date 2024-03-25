// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Objects/CallAlbertosActor.h"
#include "Components/BoxComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"
// Sets default values
ACallAlbertosActor::ACallAlbertosActor()
{
	PrimaryActorTick.bCanEverTick = false;

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

void ACallAlbertosActor::CallAlbertosBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bWasCalled || !IsValid(OtherActor))
		return;

	TObjectPtr<AMarineCharacter> Player = Cast<AMarineCharacter>(OtherActor);
	if (!IsValid(Player))
		return;
	if (!IsValid(Player->GetAlbertosPawn()))
		return;

	Player->GetAlbertosPawn()->SetActorLocation(TeleportAlbertosLocation);

	bWasCalled = true;
}

