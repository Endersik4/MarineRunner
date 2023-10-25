// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnDeathWidgetComponent.generated.h"


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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SpawnDeathWidget(APlayerController*);
	void RestartGameInYouDiedWidget();
	void QuitGameInYouDiedWidget();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Death Widget Settings")
		float DelayForGamePause = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UYouDiedWidget> YouDiedWidgetClass;

	class UYouDiedWidget* YouDiedWidget;
	FTimerHandle PauseGameHandle;
	void PauseGameAfterDelay();
};
