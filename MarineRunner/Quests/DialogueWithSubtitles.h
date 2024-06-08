// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueWithSubtitles.generated.h"

USTRUCT(BlueprintType)
struct FDialogueSubtitles {

public:
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	FText CurrentSubtitles = FText();
	UPROPERTY(EditAnywhere)
	float TimeOnScreen = 0.f;

	FDialogueSubtitles()
	{
		CurrentSubtitles = FText();
		TimeOnScreen = 0.f;
	}
	FDialogueSubtitles(const FText& _CurrentSubtitles, const float& _TimeOnScreen)
	{
		CurrentSubtitles = _CurrentSubtitles;
		TimeOnScreen = _TimeOnScreen;
	}
};

UCLASS()
class MARINERUNNER_API ADialogueWithSubtitles : public AActor
{
	GENERATED_BODY()

public:
	ADialogueWithSubtitles();

protected:
	virtual void BeginPlay() override;

public:

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UBoxComponent> ActivateDialogueBox = nullptr;

	UFUNCTION()
	void OnActivateDialogueBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Dialogue settings")
	float StartDialogueDelay = 1.f;
	UPROPERTY(EditAnywhere, Category = "Dialogue settings")
	TObjectPtr<USoundBase> DialogueSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Dialogue settings")
	TSubclassOf<class UDialogueSubtitlesWidget> DialogueWidgetClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Dialogue settings")
	TArray<FDialogueSubtitles> AllSubtitlesForDialogue;

	UPROPERTY(Transient)
	bool bWasActivated = false;

	void StartDialogue();

	UPROPERTY(Transient)
	int32 CurrentDialogueSubtitles = 0;

	FTimerHandle ChangeSubtitlesHandle;
	void ChangeSubtitles();

	// return true if widget spawned successfully
	bool SpawnDialogueWidget();
	UPROPERTY(Transient)
	TObjectPtr<UDialogueSubtitlesWidget> SpawnedDialogueWidget = nullptr;
};
