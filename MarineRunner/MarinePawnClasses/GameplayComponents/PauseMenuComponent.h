// Copyright Adam Bartela. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PauseMenuComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UPauseMenuComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPauseMenuComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	class UPauseMenuWidget* GetPauseMenuWidget() const { return PauseMenuWidget; }
	FORCEINLINE const bool& GetIsInPauseMenu() const { return bIsInPauseMenu; }

	void PauseGame();
	void UnPauseGame();

	void ChangeUIToGameOnly();
private:

	bool bIsInPauseMenu;
	class UPauseMenuWidget* PauseMenuWidget;
	void SpawnPauseMenuWidget();
	bool CanPauseGame();

	APlayerController* PlayerController;
	class AMarineCharacter* MarinePawn;
};