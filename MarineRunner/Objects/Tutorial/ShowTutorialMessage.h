// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShowTutorialMessage.generated.h"

UCLASS()
class MARINERUNNER_API AShowTutorialMessage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShowTutorialMessage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetCanShowTutorialMessage(bool bCan) { bCanShowTutorialMessage = bCan; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* ShowMessageBoxComp;
	UFUNCTION()
		void ShowMessageBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Message Settings")
		FText MessageTitle;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		FText MessageText;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		float HideMessageAfterTime = 15.f;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		TSubclassOf<class UMessageToReadWidget> MessageWidgetClass;

	bool bCanShowTutorialMessage = true;
	FTimerHandle SpawnMessageWidgetHandle;
};
