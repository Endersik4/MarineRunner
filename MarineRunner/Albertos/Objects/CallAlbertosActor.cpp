// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Objects/CallAlbertosActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/AlbertosAIController.h"
#include "MarineRunner/Albertos/Components/PlayerIsNearAlbertosComponent.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

// Sets default values
ACallAlbertosActor::ACallAlbertosActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CallAlbertosBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CallAlbertosBoxComp"));
	RootComponent = CallAlbertosBoxComp;
	CallAlbertosBoxComp->SetCollisionProfileName(FName(TEXT("DetectOnlyPlayer")));

}

// Called when the game starts or when spawned
void ACallAlbertosActor::BeginPlay()
{
	Super::BeginPlay();
	
	CallAlbertosBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACallAlbertosActor::CallAlbertosBoxBeginOverlap);
}

void ACallAlbertosActor::CallAlbertosBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bWasCalled)
		return;

	TeleportAlbertos(OtherActor);

	bWasCalled = true;

	SaveCurrentCallState();
}

void ACallAlbertosActor::TeleportAlbertos(AActor* OtherActor)
{
	if (!IsValid(OtherActor))
		return;

	TObjectPtr<AMarineCharacter> Player = Cast<AMarineCharacter>(OtherActor);
	if (!IsValid(Player))
		return;

	TObjectPtr<AAlbertosPawn> AlbertosPawn = Player->GetAlbertosPawn();
	if (!IsValid(AlbertosPawn))
		return;

	StopAlbertosMovement(Player->GetAlbertosPawn(), true);

	FTimerDelegate ResumeMovementDelegate = FTimerDelegate::CreateUObject(this, &ACallAlbertosActor::StopAlbertosMovement, Player->GetAlbertosPawn(), false);
	GetWorld()->GetTimerManager().SetTimer(ResumeAlbertosMovementHandle, ResumeMovementDelegate, ResumeAlbertosMovementTime, false);

	AlbertosPawn->SetActorLocation(TeleportAlbertosLocation);
	AlbertosPawn->SetActorRotation(TeleportAlbertosRotation);
}

void ACallAlbertosActor::StopAlbertosMovement(TObjectPtr<class AAlbertosPawn> AlbertosPawn, bool bStop)
{
	TObjectPtr<AAlbertosAIController> AlbertosAIController = Cast<AAlbertosAIController>(AlbertosPawn->GetController());
	if (!IsValid(AlbertosAIController))
		return;

	if (AlbertosPawn->GetPlayerIsNearComponent()->GetPlayerIsClose() && !bStop)
		return;

	AlbertosAIController->SetCanMove(!bStop);

	if (bStop)
		AlbertosAIController->StopMovement();
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
