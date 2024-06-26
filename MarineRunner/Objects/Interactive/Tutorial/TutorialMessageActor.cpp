// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Interactive/Tutorial/TutorialMessageActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Objects/Interactive/MessageToRead/MessageToReadWidget.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/MessageHandlerComponent.h"
#include "MarineRunner/Player/SaveLoadGame/SaveLoadPlayerComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"


AShowTutorialMessage::AShowTutorialMessage()
{
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
	if (!bCanShowTutorialMessage)
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance))
	{
		if (GameInstance->IsPlayerInCombat())
			return;
	}

	UnlockGameplayMechanicsInHud(OtherActor);

	TObjectPtr<AMarineCharacter> MarinePlayer = Cast<AMarineCharacter>(OtherActor);
	if (!IsValid(MarinePlayer))
		return;

	TObjectPtr<UUserWidget> TempMessageWidget = MarinePlayer->GetMessageHandlerComponent()->SpawnWidget(MessageWidgetClass, true);
	if (!IsValid(TempMessageWidget))
		return;

	TObjectPtr<UMessageToReadWidget> MessageWidget = Cast<UMessageToReadWidget>(TempMessageWidget);
	if (!IsValid(MessageWidget))
		return;

	MessageWidget->SetMessageInformation(MessageTitle, MessageText);
	MessageWidget->HideMessageAfterTime(HideMessageAfterTime);

	EnableShowTutorialMessage(false);
	MessageReadedSaveData();
}

void AShowTutorialMessage::EnableShowTutorialMessage(bool bEnable)
{
	SetActorEnableCollision(bEnable);
	SetActorTickEnabled(bEnable);
	bCanShowTutorialMessage = bEnable;
}

void AShowTutorialMessage::UnlockGameplayMechanicsInHud(TObjectPtr<AActor> Player)
{
	if (!bUnlockGameplayMechanicsInHud || !IsValid(Player))
		return;

	TObjectPtr<AMarineCharacter> MarinePlayer = Cast<AMarineCharacter>(Player);
	if (!IsValid(MarinePlayer))
		return;
	if (!IsValid(MarinePlayer->GetSaveLoadPlayerComponent()))
		return;

	MarinePlayer->GetSaveLoadPlayerComponent()->ShowGameplayMechanicsOnHud(UnlockInHud);
}

void AShowTutorialMessage::MessageReadedSaveData()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));
	if (!IsValid(SavedDataObject))
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
	EnableShowTutorialMessage(true);
}
