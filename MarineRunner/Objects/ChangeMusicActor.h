// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "ChangeMusicActor.generated.h"

UCLASS()
class MARINERUNNER_API AChangeMusicActor : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChangeMusicActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

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

	void ChangedMusicSaveData();

	int32 CurrentUniqueID = 0;
};
