// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageHandlerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FCloseMessageDelegate, class AMarineCharacter*)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UMessageHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMessageHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	FORCEINLINE void SetCurrentDisplayedMessage(TObjectPtr<class UMessageToReadWidget> DisplayedMessage) { CurrentDisplayedMessage = DisplayedMessage; }
	FORCEINLINE void SetIsMessageDisplayed(bool _bMessageDisplayed) { bIsMessageDisplayed = _bMessageDisplayed; }

	FORCEINLINE bool GetIsMessageDisplayed() const { return bIsMessageDisplayed; }
	FORCEINLINE bool GetIsCheatsDisplayed() const { return bIsCheatsDisplayed; }

	void DeleteCurrentDisplayedMessage(class AMarineCharacter*);

	void SpawnNewRecipeUnlockedWidget();
	void SpawnNotEnoughSlotsForWeaponWidget();

	void SpawnCheatsWidget();

	// spawns widget and when bOnlyOneAtTime == true then when another widget is spawned the first one will be deleted
	TObjectPtr<UUserWidget> SpawnWidget(const TSubclassOf<UUserWidget>& WidgetClassToSpawn, bool bOnlyOneAtTime = false);


private:
	UPROPERTY(EditDefaultsOnly, Category = "Messages Handler")
	TSubclassOf<UUserWidget> NewRecipeUnlockedClassWidget = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Messages Handler")
	TSubclassOf<UUserWidget> NotEnoughSlotsForWeaponClassWidget = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Messages Handler")
	TObjectPtr<USoundBase> MessagePopUpSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Cheats")
	TSubclassOf<class UCheatWidget> CheatWidgetClass = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> SpawnedOneAtTimeWidget = nullptr;

	UPROPERTY(Transient)
	bool bIsMessageDisplayed = false;
	UPROPERTY(Transient)
	TObjectPtr<class UMessageToReadWidget> CurrentDisplayedMessage = nullptr;

	UPROPERTY(Transient)
	bool bIsCheatsDisplayed = false;
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> SpawnedCheatWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
};
