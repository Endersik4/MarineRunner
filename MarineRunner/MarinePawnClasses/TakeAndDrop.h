// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TakeAndDrop.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

private:
	UPROPERTY(EditAnywhere, Category = "Take Items")
		float TakeDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = "Gun options")
		FVector CollisionBoxSize = FVector(20.f);

	class AMarineCharacter* MarinePawn;
	class ITakeInterface* TakeInterface;

	bool RaycastForHoverItems();
		
	FHitResult LastHitResult;
	bool WhetherRaycastOnTheSameItem(const FHitResult& CurrentItemHit);
	void HoverHitItem(const bool& bWasHit, const FHitResult& ItemHit);
	void DisableLastHoveredItem();
};
