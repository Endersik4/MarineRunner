// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MarineRunnerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API AMarineRunnerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(Exec, Category = ExecFunctions)
	void ChangeMap(const FName& NewLevelName);

	UFUNCTION(Exec, Category = ExecFunctions)
	void AddItem(const FName ItemRowNameToAdd, const int32 AmountToAdd);

	UFUNCTION(Exec, Category = ExecFunctions)
	void TeleportToNextStage(); //ztgk, delete later

	UFUNCTION(Exec, Category = ExecFunctions)
	void ApplyDifficultyLevelToAllEnemies();

	UPROPERTY(EditAnywhere)
	TArray<FVector> StagesToTeleport;

private:
	TObjectPtr<class AMarineCharacter> GetPlayer();
};
