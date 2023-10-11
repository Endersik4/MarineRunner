// Copyright Adam Bartela. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PauseMenuComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	class UPauseMenuWidget* GetPauseMenuWidget() const { return PauseMenuWidget; }

	void CallUnPauseGame() { UnPauseGame(); }
	void PauseGame();
private:
	class UPauseMenuWidget* PauseMenuWidget;
	void UnPauseGame();
	void SpawnPauseMenuWidget();

	APlayerController* PlayerController;
};
