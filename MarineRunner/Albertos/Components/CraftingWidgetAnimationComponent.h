// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftingWidgetAnimationComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UCraftingWidgetAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCraftingWidgetAnimationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE const bool GetIsCraftingWidgetAnimationPlaying() const { return bIsCraftingWidgetAnimatinPlaying; }
	void PrepareCraftingWidgetAnimation(bool bForwardAnim);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Crafting Widget Animation")
	float CraftingWidgetAnimationTime = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting Widget Animation")
	FVector DissolveBoxesOffsetForAnim = FVector(0.f, 2130.f, 0.f);

	// Animation for opening/closing crafting table widget

	UPROPERTY(Transient)
	bool bIsCraftingWidgetAnimatinPlaying = false;
	UPROPERTY(Transient)
	bool bWasCraftingWidgetClosed = false;
	UPROPERTY(Transient)
	float CraftingWidgetAnimationTimeElapsed = 0.f;
	UPROPERTY(Transient)
	FVector DissolveBoxStartLoc_L = FVector::Zero();
	UPROPERTY(Transient)
	FVector DissolveBoxEndLoc_L = FVector::Zero();
	UPROPERTY(Transient)
	FVector DissolveBoxStartLoc_R = FVector::Zero();
	UPROPERTY(Transient)
	FVector DissolveBoxEndLoc_R = FVector::Zero();
	void CraftingWidgetAnimationEnded();
	void ToggleVisibilityForDissolveBoxes();
	void CraftingWidgetAnimation(float Delta);

	UPROPERTY(Transient)
	TObjectPtr<class AAlbertosPawn> AlbertosOwner = nullptr;
};
