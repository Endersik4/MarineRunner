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

private:
	UPROPERTY(EditDefaultsOnly, Category = "MainMenu")
		TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass;

	void BackToPreviousMenu();

	bool bIsInMainMenu;
	void SpawnMainMenuWidget();

	UPROPERTY(Transient)
		TObjectPtr<class UMainMenuWidget> MainMenuWidget;
};
