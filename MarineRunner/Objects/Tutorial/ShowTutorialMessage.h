// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "ShowTutorialMessage.generated.h"

UCLASS()
class MARINERUNNER_API AShowTutorialMessage : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShowTutorialMessage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:	

	FORCEINLINE void SetCanShowTutorialMessage(bool bCan) { bCanShowTutorialMessage = bCan; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* ShowMessageBoxComp;
	UFUNCTION()
		void ShowMessageBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Message Settings")
		FText MessageTitle;
	UPROPERTY(EditAnywhere, Category = "Message Settings", meta = (MultiLine = "true"))
		FText MessageText;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		float HideMessageAfterTime = 15.f;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		TSubclassOf<class UMessageToReadWidget> MessageWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Message Settings")
		bool bUnlockGameplayMechanicsInHud = false;
	UPROPERTY(EditAnywhere, Category = "Message Settings", meta = (EditCondition = "bUnlockGameplayMechanicsInHud", EditConditionHides))
		TEnumAsByte<enum EUnlockInHud> UnlockInHud;

	bool bCanShowTutorialMessage = true;
	FTimerHandle SpawnMessageWidgetHandle;

	void EnableShowTutorialMessage(bool bDisable);

	void UnlockGameplayMechanicsInHud(TObjectPtr<AActor> Player);

	void MessageReadedSaveData();
	int32 CurrentUniqueID = 0;
};
