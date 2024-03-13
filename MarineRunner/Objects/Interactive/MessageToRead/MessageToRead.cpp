// Copyright Adam Bartela.All Rights Reserved


#include "MessageToRead.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/Components/MessageHandlerComponent.h"
#include "MessageToReadWidget.h"

AMessageToRead::AMessageToRead()
{
	PrimaryActorTick.bCanEverTick = false;

	MessageStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MessageStaticMesh"));
	RootComponent = MessageStaticMesh;

	MessageStaticMesh->SetSimulatePhysics(true);
	MessageStaticMesh->SetCollisionProfileName(TEXT("ItemCollision"));
	MessageStaticMesh->SetNotifyRigidBodyCollision(true);
	MessageStaticMesh->SetGenerateOverlapEvents(false);
	MessageStaticMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	MessageStaticMesh->bRenderCustomDepth = false; //Its for outline material 
}

// Called when the game starts or when spawned
void AMessageToRead::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMessageToRead::TakeItem(AMarineCharacter* Character)
{
	if (Character->GetIsInSlowMotion())
		return;

	Player = Character;

	GetWorld()->GetTimerManager().SetTimer(SpawnMessageWidgetHandle, this, &AMessageToRead::SpawnMessageWidget, OpenMessageDelay);
}

void AMessageToRead::ItemHover(AMarineCharacter* Character)
{
	if (!IsValid(Character->GetHudWidget())) 
		return;

	MessageStaticMesh->SetRenderCustomDepth(true);
	Character->GetHudWidget()->SetItemHoverInformations(MessageName, MessageDescription, MessageIcon);
}

void AMessageToRead::ItemUnHover(AMarineCharacter* Character)
{
	if (!IsValid(Character->GetHudWidget()))
		return;

	MessageStaticMesh->SetRenderCustomDepth(false);
	Character->GetHudWidget()->PlayAppearAnimForItemHover(false);
}

void AMessageToRead::SpawnMessageWidget()
{
	if (!IsValid(Player)|| !IsValid(MessageWidgetClass))
		return;

	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PlayerController))
		return;

	TObjectPtr<UMessageToReadWidget> MessageWidget = Cast<UMessageToReadWidget>(CreateWidget(PlayerController, MessageWidgetClass));
	if (!IsValid(MessageWidget))
		return;

	MessageWidget->AddToViewport();
	MessageWidget->SetMessageInformation(MessageTitle, MessageText);

	Player->GetMessageHandlerComponent()->SetCurrentDisplayedMessage(MessageWidget);
	Player->GetMessageHandlerComponent()->SetIsMessageDisplayed(true);

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, MessageWidget);
	PlayerController->SetShowMouseCursor(true);

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

