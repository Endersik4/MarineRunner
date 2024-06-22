// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "SoundEffectActor.generated.h"

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

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> ChangeMusicBoxComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Music")
	bool bChangeMusic = true;
	UPROPERTY(EditAnywhere, Category = "Music")
	bool bPlaySound = false;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (EditCondition = "bChangeMusic", EditConditionHides))
	TObjectPtr<USoundBase> MusicToChange = nullptr;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (EditCondition = "bPlaySound", EditConditionHides))
	TObjectPtr<USoundBase> SoundToPlay = nullptr;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (EditCondition = "bChangeMusic", EditConditionHides))
	bool bAddDelayToMusic = false;
	UPROPERTY(EditAnywhere, Category = "Music", meta = (EditCondition = "bChangeMusic && bAddDelayToMusic", EditConditionHides))
	float MusicDelay = 2.f;
	UFUNCTION()
	void ChangeMusicSoundBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DisableChangeMusic(bool bDisable = true);
	void ChangeBackgroundMusic();

	void ChangedMusicSaveData();

	UPROPERTY(Transient)
	bool OriginalChangeMusic = false;
	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;

	UPROPERTY(Transient)
	FTimerHandle MusicDelayHandle = FTimerHandle();
};
