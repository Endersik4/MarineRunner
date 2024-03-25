// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API USpawnDeathWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnDeathWidgetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void SpawnDeathWidget(TObjectPtr<APlayerController> PlayerController);
	void RestartGameInYouDiedWidget();
	void QuitGameInYouDiedWidget();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Death Widget Settings")
		float DelayForGamePause = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Death Widget Settings")
		TSubclassOf<class UYouDiedWidget> YouDiedWidgetClass = nullptr;

	UPROPERTY(Transient)
		TObjectPtr<class UYouDiedWidget> YouDiedWidget = nullptr;

	FTimerHandle PauseGameHandle;
	void PauseGameAfterDelay();
};
