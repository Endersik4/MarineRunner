// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

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
		TObjectPtr<class UStaticMeshComponent> MessageStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Message Settings")
		FText MessageTitle;
	UPROPERTY(EditAnywhere, Category = "Message Settings", meta = (MultiLine = "true"))
		FText MessageText;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		float OpenMessageDelay = 0.f;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		TSubclassOf<class UMessageToReadWidget> MessageWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Message Settings|Information On Hover")
		FString MessageName;
	UPROPERTY(EditDefaultsOnly, Category = "Message Settings|Information On Hover")
		FString MessageDescription;
	UPROPERTY(EditDefaultsOnly, Category = "Message Settings|Information On Hover")
		TObjectPtr<UTexture2D> MessageIcon;

	FTimerHandle SpawnMessageWidgetHandle;
	void SpawnMessageWidget();

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;
};
