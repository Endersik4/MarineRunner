// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TakeComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UTakeAndDrop : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTakeAndDrop();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Take();
	void TakeReleased();

	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);
private:
	UPROPERTY(EditAnywhere, Category = "Take Items")
	float TakeDistance = 500.f;
	UPROPERTY(EditAnywhere, Category = "Take Items")
	FVector CollisionBoxSize = FVector(30.f);
	UPROPERTY(EditAnywhere, Category = "Take Items")
	float TakeAnotherItemTime = 0.12f;

	bool RaycastForHoverItems();

	FTimerHandle ConstantlyTakeHandle;

	FHitResult LastHitResult;
	bool WhetherRaycastOnTheSameItem(const FHitResult& CurrentItemHit);
	void HoverHitItem(const bool& bWasHit, const FHitResult& ItemHit);
	void DisableLastHoveredItem();

	class ITakeInterface* TakeInterface = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> MarinePawn = nullptr;
};