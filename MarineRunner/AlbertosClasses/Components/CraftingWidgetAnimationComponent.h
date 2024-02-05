// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftingWidgetAnimationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	FORCEINLINE const bool GetIsCraftingWidgetAnimatinPlaying() const { return bIsCraftingWidgetAnimatinPlaying; }
	void PrepareCraftingWidgetAnimation(bool bForwardAnim);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Crafting Widget Animation")
		float CraftingWidgetAnimationTime = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting Widget Animation")
		FVector DissolveBoxesOffsetForAnim = FVector(0.f, 2130.f, 0.f);

	// Animation for opening/closing crafting table widget
	bool bIsCraftingWidgetAnimatinPlaying;
	bool bWasCraftingWidgetClosed;
	float CraftingWidgetAnimationTimeElapsed;
	FVector DissolveBox_Left_StartLoc;
	FVector DissolveBox_Left_EndLoc;
	FVector DissolveBox_Right_StartLoc;
	FVector DissolveBox_Right_EndLoc;
	void CraftingWidgetAnimationEnded();
	void ToggleVisibilityForDissolveBoxes();
	void CraftingWidgetAnimation(float Delta);

	class AAlbertosPawn* AlbertosOwner;
};
