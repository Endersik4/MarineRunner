// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChangeMusicActor.generated.h"

UCLASS()
class MARINERUNNER_API AChangeMusicActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChangeMusicActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Music", meta = (InlineEditConditionToggle))
		bool bChangeMusic = true;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (InlineEditConditionToggle))
		bool bPlaySound;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (EditCondition = "bChangeMusic"))
		USoundBase* MusicToChange;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (EditCondition = "bPlaySound"))
		USoundBase* SoundToPlay;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* ChangeMusicBoxComp;
	UFUNCTION()
		void ChangeMusicBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DisableChangeMusic();
	void ChangeBackgroundMusic();
};
