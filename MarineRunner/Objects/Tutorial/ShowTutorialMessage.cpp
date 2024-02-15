// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Tutorial/ShowTutorialMessage.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Objects/MessageToReadWidget.h"
#include "MarineRunner/Objects/SavedDataObject.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

// Sets default values
AShowTutorialMessage::AShowTutorialMessage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ShowMessageBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Show Message Box Comp"));
	RootComponent = ShowMessageBoxComp;
	ShowMessageBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShowMessageBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AShowTutorialMessage::BeginPlay()
{
	Super::BeginPlay();

	ShowMessageBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AShowTutorialMessage::ShowMessageBoxBeginOverlap);
}

void AShowTutorialMessage::ShowMessageBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (MessageWidgetClass == nullptr || bCanShowTutorialMessage == false)
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC) == false)
		return;

	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance))
	{
		if (GameInstance->IsPlayerInCombat() == true)
			return;
	}
	
	UMessageToReadWidget* MessageWidget = Cast<UMessageToReadWidget>(CreateWidget(PC, MessageWidgetClass));
	if (IsValid(MessageWidget) == false)
		return;

	MessageWidget->AddToViewport();
	MessageWidget->SetMessageInformation(MessageTitle, MessageText);
	MessageWidget->HideMessageAfterTime(HideMessageAfterTime);

	bCanShowTutorialMessage = false;
	MessageReadedSaveData();
}

void AShowTutorialMessage::MessageReadedSaveData()
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void AShowTutorialMessage::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	if (SavedCustomData.ObjectState != 1)
		return;

	bCanShowTutorialMessage = false;
}

void AShowTutorialMessage::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AShowTutorialMessage::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}
