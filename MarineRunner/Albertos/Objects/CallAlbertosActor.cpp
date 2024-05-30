// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Objects/CallAlbertosActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

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

	SaveCurrentCallState();
}

void ACallAlbertosActor::SaveCurrentCallState()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void ACallAlbertosActor::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState == 1)
		bWasCalled = true;
}

void ACallAlbertosActor::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void ACallAlbertosActor::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	bWasCalled = false;
}
