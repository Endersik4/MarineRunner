// Copyright Epic Games, Inc. All Rights Reserved.


#include "MarineRunnerGameModeBase.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "MarineRunner/Enemies/TypesOfEnemy/ShootingEnemyPawn.h"
#include "MarineRunnerGameInstance.h"

#pragma region //////// CUSTOM COMMANDS ///////
void AMarineRunnerGameModeBase::ChangeMap(const FName& NewLevelName)
{
	DestroyAllEnemies();
	ResetDetectedEnemies();

	UGameplayStatics::OpenLevel(GetWorld(), NewLevelName);
}

void AMarineRunnerGameModeBase::DestroyAllEnemies()
{
	TArray<TObjectPtr<AActor>> AllEnemiesOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass(), AllEnemiesOnMap);

	for (TObjectPtr<AActor> CurrentEnemy : AllEnemiesOnMap)
	{
		if (!IsValid(CurrentEnemy))
			continue;

		CurrentEnemy->Destroy();
	}
}

void AMarineRunnerGameModeBase::ResetDetectedEnemies()
{
	TObjectPtr<UGameInstance> TempGameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!IsValid(TempGameInstance))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(TempGameInstance);
	if (!IsValid(GameInstance))
		return;
	GameInstance->bNewGame = true;
	GameInstance->ResetDetectedEnemy();
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

	DestroyAllEnemies();
	ResetDetectedEnemies();

	Player->SetActorLocation(StagesToTeleport[Player->CheckpointNumber]);
}

void AMarineRunnerGameModeBase::ApplyDifficultyLevelToAllEnemies()
{
	TArray<TObjectPtr<AActor>> AllShootingEnemiesOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShootingEnemyPawn::StaticClass(), AllShootingEnemiesOnMap);
	for (TObjectPtr<AActor> CurrentEnemy : AllShootingEnemiesOnMap)
	{
		if (!IsValid(CurrentEnemy))
			continue;

		TObjectPtr<AShootingEnemyPawn> ShootingEnemy = Cast<AShootingEnemyPawn>(CurrentEnemy);
		if (!IsValid(ShootingEnemy))
			continue;

		ShootingEnemy->ApplyEnemyDifficulty();
	}
}

TObjectPtr<AMarineCharacter> AMarineRunnerGameModeBase::GetPlayer()
{
	TObjectPtr<APawn> TempPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(TempPlayerPawn))
		return nullptr;

	return Cast<AMarineCharacter>(TempPlayerPawn);
}

#pragma endregion

