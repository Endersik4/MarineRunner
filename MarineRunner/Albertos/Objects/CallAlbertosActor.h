// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "CallAlbertosActor.generated.h"

UCLASS()
class MARINERUNNER_API ACallAlbertosActor : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACallAlbertosActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> CallAlbertosBoxComp;

	UPROPERTY(EditAnywhere, Category = "Call Albertos Settings")
	FVector TeleportAlbertosLocation = FVector::Zero();
	UPROPERTY(EditAnywhere, Category = "Call Albertos Settings")
	FRotator TeleportAlbertosRotation = FRotator(0.f);
	UPROPERTY(EditAnywhere, Category = "Call Albertos Settings")
	float ResumeAlbertosMovementTime = 5.f;

	UFUNCTION()
	void CallAlbertosBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(Transient)
	bool bWasCalled = false;

	void TeleportAlbertos(AActor* OtherActor);
	void StopAlbertosMovement(TObjectPtr<class AAlbertosPawn> AlbertosPawn, bool bStop = true);

	UPROPERTY(Transient)
	FTimerHandle ResumeAlbertosMovementHandle = FTimerHandle();

	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;

	void SaveCurrentCallState();
};
