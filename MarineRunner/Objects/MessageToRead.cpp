// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/MessageToRead.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Objects/MessageToReadWidget.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Sets default values
AMessageToRead::AMessageToRead()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void AMessageToRead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMessageToRead::TakeItem(AMarineCharacter* Character, bool& bIsItWeapon)
{
	bIsItWeapon = false;
	if (Character->GetIsInSlowMotion() == true)
		return;

	MarinePlayer = Character;

	GetWorld()->GetTimerManager().SetTimer(SpawnMessageWidgetHandle, this, &AMessageToRead::SpawnMessageWidget, OpenMessageDelay);
}

void AMessageToRead::ItemHover(UHUDWidget* MarineHUDWidget)
{
	MessageStaticMesh->SetRenderCustomDepth(true);

	if (MarineHUDWidget == nullptr) return;
	MarineHUDWidget->SetItemHoverInformations(MessageName, MessageDescription, MessageIcon);
}

void AMessageToRead::ItemUnHover(UHUDWidget* MarineHUDWidget)
{
	MessageStaticMesh->SetRenderCustomDepth(false);

	if (MarineHUDWidget == nullptr) return;
	MarineHUDWidget->PlayAppearAnimForItemHover(false);
}

void AMessageToRead::SpawnMessageWidget()
{
	if (IsValid(MarinePlayer) == false || MessageWidgetClass == nullptr)
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC) == false)
		return;

	UMessageToReadWidget* MessageWidget = Cast<UMessageToReadWidget>(CreateWidget(PC, MessageWidgetClass));
	if (IsValid(MessageWidget) == false)
		return;

	MessageWidget->AddToViewport();
	MessageWidget->SetMessageInformation(MessageTitle, MessageText);

	MarinePlayer->GetMessageHandlerComponent()->SetCurrentDisplayedMessage(MessageWidget);
	MarinePlayer->GetMessageHandlerComponent()->SetIsMessageDisplayed(true);

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, MessageWidget);
	PC->SetShowMouseCursor(true);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

