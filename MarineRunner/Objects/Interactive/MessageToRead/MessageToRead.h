// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/Interfaces/TakeInterface.h"

#include "MessageToRead.generated.h"

UCLASS()
class MARINERUNNER_API AMessageToRead : public AActor, public  ITakeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMessageToRead();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void TakeItem(class AMarineCharacter* Character) override;
	virtual void ItemHover(class AMarineCharacter* Character) override;
	virtual void ItemUnHover(class AMarineCharacter* Character) override;
public:	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> MessageStaticMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Message Settings")
		FText MessageTitle = FText();
	UPROPERTY(EditAnywhere, Category = "Message Settings", meta = (MultiLine = "true"))
		FText MessageText = FText();
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		float OpenMessageDelay = 0.f;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		TSubclassOf<class UMessageToReadWidget> MessageWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Message Settings|Information On Hover")
		FString MessageName = FString("default message name");
	UPROPERTY(EditDefaultsOnly, Category = "Message Settings|Information On Hover")
		FString MessageDescription = FString("default message description");
	UPROPERTY(EditDefaultsOnly, Category = "Message Settings|Information On Hover")
		TObjectPtr<UTexture2D> MessageIcon = nullptr;

	FTimerHandle SpawnMessageWidgetHandle;
	void SpawnMessageWidget();

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player = nullptr;
};
