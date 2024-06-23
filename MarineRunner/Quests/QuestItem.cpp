// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Quests/QuestItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"

AQuestItem::AQuestItem()
{
	PrimaryActorTick.bCanEverTick = false;

	QuestItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Quest Item Mesh"));
	RootComponent = QuestItemMesh;
}

void AQuestItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AQuestItem::TakeItem(AMarineCharacter* Player)
{
	if (!IsValid(Player))
		return;

	if (bQuestWasPlayed)
		return;

	if (Player->GetWeaponHandlerComponent()->GetIsPlayerInAds())
		return;

	AddActorToSavedDataObject();
	ItemUnHover(Player);
	bQuestWasPlayed = true;

	Player->SetIsInCutscene(true);
	Player->ShowHUD(false);

	const float TimeToStartQuest = Player->GetWeaponHandlerComponent()->HideWeaponByPlayer() + 0.01f;
	FTimerDelegate QuestDelegate = FTimerDelegate::CreateUObject(this, &AQuestItem::StartQuestAnim, Player);
	GetWorld()->GetTimerManager().SetTimer(StartQuestAnimHandle, QuestDelegate, TimeToStartQuest, false);
}

void AQuestItem::StartQuestAnim(AMarineCharacter* Player)
{
	if (!IsValid(Player))
		return;

	QuestItemMesh->SetVisibility(false);

	const FTransform& QuestItemTransform = FTransform(FRotator(0.f), FVector(0.f), FVector(1.f));
	SpawnedQuestActor = GetWorld()->SpawnActor<AActor>(QuestActorToAttach, QuestItemTransform);
	if (!IsValid(SpawnedQuestActor))
		return;

	SpawnedQuestActor->AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachItemToSocketName);
	
	if (!IsValid(QuestAnimation))
		return;

	Player->GetArmsSkeletalMesh()->SetForceRefPose(false);
	Player->GetArmsSkeletalMesh()->PlayAnimation(QuestAnimation, false);

	FTimerDelegate EndQuestDelegate = FTimerDelegate::CreateUObject(this, &AQuestItem::EndQuestAnim, Player);
	GetWorld()->GetTimerManager().SetTimer(EndQuestAnimHandle, EndQuestDelegate, QuestAnimation->GetPlayLength(), false);
}

void AQuestItem::EndQuestAnim(AMarineCharacter* Player)
{
	if (IsValid(SpawnedQuestActor))
		SpawnedQuestActor->Destroy();

	QuestItemMesh->SetVisibility(true);

	if (!IsValid(Player))
		return;

	Player->SetIsInCutscene(false);
	Player->GetArmsSkeletalMesh()->SetForceRefPose(true);
	Player->ShowHUD(true);
	Player->GetWeaponHandlerComponent()->SelectWeaponFromQuickInventory(1);
}

void AQuestItem::ItemHover(AMarineCharacter* Character)
{
	if (bQuestWasPlayed)
		return;

	QuestItemMesh->SetRenderCustomDepth(true);

	if (!IsValid(Character))
		return;

	if (!IsValid(Character->GetHudWidget()))
		return;

	Character->GetHudWidget()->SetItemHoverInformations(MessageName, MessageDescription, MessageIcon);
}

void AQuestItem::ItemUnHover(AMarineCharacter* Character)
{
	if (bQuestWasPlayed)
		return;

	QuestItemMesh->SetRenderCustomDepth(false);

	if (!IsValid(Character))
		return;

	if (!IsValid(Character->GetHudWidget()))
		return;

	Character->GetHudWidget()->PlayAppearAnimForItemHover(false);
}
#pragma region ////////////// SaveCustomDataInterface Functions //////////
void AQuestItem::AddActorToSavedDataObject()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	if (CurrentUniqueID == 0)
		CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();

	SavedDataObject->AddCustomSaveData(CurrentUniqueID, FCustomDataSaved(ESavedDataState::ESDS_LoadData, this, 1));
}

void AQuestItem::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;

	if (SavedCustomData.ObjectState == 1)
		bQuestWasPlayed = true;
}

void AQuestItem::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
}

void AQuestItem::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	bQuestWasPlayed = false; 
}
#pragma endregion
