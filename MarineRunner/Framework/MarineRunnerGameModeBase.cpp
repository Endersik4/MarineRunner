// Copyright Epic Games, Inc. All Rights Reserved.


#include "MarineRunnerGameModeBase.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"

#pragma region //////// CUSTOM COMMANDS ///////
void AMarineRunnerGameModeBase::ChangeMap(const FName& NewLevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), NewLevelName);
}

void AMarineRunnerGameModeBase::AddItem(const FName ItemRowNameToAdd, const int32 AmountToAdd)
{
	TObjectPtr<AMarineCharacter> Player = GetPlayer();
	if (!IsValid(Player))
		return;

	if (!IsValid(Player->GetInventoryComponent()))
		return;

	Player->GetInventoryComponent()->AddItemToInventory(ItemRowNameToAdd, AmountToAdd);
}

void AMarineRunnerGameModeBase::TeleportToNextStage()
{
	TObjectPtr<AMarineCharacter> Player = GetPlayer();
	if (!IsValid(Player))
		return;

	if (Player->CheckpointNumber >= StagesToTeleport.Num() || Player->CheckpointNumber < 0)
		return;

	Player->SetActorLocation(StagesToTeleport[Player->CheckpointNumber]);
}

TObjectPtr<AMarineCharacter> AMarineRunnerGameModeBase::GetPlayer()
{
	TObjectPtr<APawn> TempPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(TempPlayerPawn))
		return nullptr;

	return Cast<AMarineCharacter>(TempPlayerPawn);
}
#pragma endregion

