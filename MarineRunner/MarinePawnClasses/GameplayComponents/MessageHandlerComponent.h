// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageHandlerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FCloseMessageDelegate, class AMarineCharacter*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	FORCEINLINE void SetCurrentDisplayedMessage(class UMessageToReadWidget* DisplayedMessage) { CurrentDisplayedMessage = DisplayedMessage; }
	FORCEINLINE void SetIsMessageDisplayed(bool _bMessageDisplayed) { bIsMessageDisplayed = _bMessageDisplayed; }

	FORCEINLINE bool GetIsMessageDisplayed() const { return bIsMessageDisplayed; }

	void DeleteCurrentDisplayedMessage(class AMarineCharacter*);

	void SpawnNewRecipeUnlockedWidget();
	void SpawnNotEnoughSlotsForWeaponWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Messages")
		TSubclassOf<UUserWidget> NewRecipeUnlockedClassWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Messages")
		TSubclassOf<UUserWidget> NotEnoughSlotsForWeaponClassWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* MessagePopUpSound;
private:
	bool bIsMessageDisplayed;
	class UMessageToReadWidget* CurrentDisplayedMessage;

	void SpawnWidget(const TSubclassOf<UUserWidget> & WidgetClassToSpawn);
};
