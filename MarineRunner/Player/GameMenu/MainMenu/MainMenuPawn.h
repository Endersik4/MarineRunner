// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainMenuPawn.generated.h"

UCLASS()
class MARINERUNNER_API AMainMenuPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMainMenuPawn();

protected:
	virtual void BeginPlay() override;

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetCanPressAnyKeyToContinue(bool bCan) { bCanPressAnyKeyToContinue = bCan; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<class USelectDifficultyWidget> SelectDifficultyWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	float SpawnDifficultyWidgetTime = 2.f;

	void BackToPreviousMenu();

	UPROPERTY(Transient)
	bool bIsInMainMenu = false;
	void SpawnMainMenuWidget();

	UPROPERTY(Transient)
	bool bCanPressAnyKeyToContinue = false;
	void ContinueToSelectDifficulty();
	void SpawnDifficultyWidget();

	UPROPERTY(Transient)
	TObjectPtr<class UMainMenuWidget> SpawnedMainMenuWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
};
