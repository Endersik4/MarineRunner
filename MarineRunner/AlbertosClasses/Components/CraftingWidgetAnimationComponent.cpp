// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/AlbertosClasses/Components/CraftingWidgetAnimationComponent.h"

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"


UCraftingWidgetAnimationComponent::UCraftingWidgetAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCraftingWidgetAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	AlbertosOwner = Cast<AAlbertosPawn>(GetOwner());
	
}

// Called every frame
void UCraftingWidgetAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CraftingWidgetAnimation(DeltaTime);
}

#pragma region /////////////// CRAFTING WIDGET ANIMATION ///////////////
void UCraftingWidgetAnimationComponent::CraftingWidgetAnimation(float Delta)
{
	if (bIsCraftingWidgetAnimatinPlaying == false)
		return;

	if (CraftingWidgetAnimationTimeElapsed >= CraftingWidgetAnimationTime)
	{
		CraftingWidgetAnimationEnded();
	}

	FVector NewBoxLeftLoc = FMath::Lerp(DissolveBox_Left_StartLoc, DissolveBox_Left_EndLoc, CraftingWidgetAnimationTimeElapsed / CraftingWidgetAnimationTime);
	FVector NewBoxRightLoc = FMath::Lerp(DissolveBox_Right_StartLoc, DissolveBox_Right_EndLoc, CraftingWidgetAnimationTimeElapsed / CraftingWidgetAnimationTime);

	AlbertosOwner->DissolveBox_Left->SetRelativeLocation(NewBoxLeftLoc);
	AlbertosOwner->DissolveBox_Right->SetRelativeLocation(NewBoxRightLoc);

	CraftingWidgetAnimationTimeElapsed += Delta;
}

void UCraftingWidgetAnimationComponent::CraftingWidgetAnimationEnded()
{
	ToggleVisibilityForDissolveBoxes();
	bIsCraftingWidgetAnimatinPlaying = false;

	if (bWasCraftingWidgetClosed == true)
	{
		AlbertosOwner->ToggleVisibilityCraftingWidget();
		bWasCraftingWidgetClosed = false;
	}
}

void UCraftingWidgetAnimationComponent::PrepareCraftingWidgetAnimation(bool bForwardAnim)
{
	bIsCraftingWidgetAnimatinPlaying = true;

	DissolveBox_Left_StartLoc = AlbertosOwner->DissolveBox_Left->GetRelativeLocation();
	DissolveBox_Right_StartLoc = AlbertosOwner->DissolveBox_Right->GetRelativeLocation();

	CraftingWidgetAnimationTimeElapsed = 0.f;

	if (bForwardAnim)
	{
		DissolveBox_Left_EndLoc = DissolveBox_Left_StartLoc + DissolveBoxesOffsetForAnim;
		DissolveBox_Right_EndLoc = DissolveBox_Right_StartLoc - DissolveBoxesOffsetForAnim;
	}
	else
	{
		DissolveBox_Left_EndLoc = DissolveBox_Left_StartLoc - DissolveBoxesOffsetForAnim;
		DissolveBox_Right_EndLoc = DissolveBox_Right_StartLoc + DissolveBoxesOffsetForAnim;
		bWasCraftingWidgetClosed = true;
	}

	ToggleVisibilityForDissolveBoxes();
}

void UCraftingWidgetAnimationComponent::ToggleVisibilityForDissolveBoxes()
{
	AlbertosOwner->DissolveBox_Left->ToggleVisibility();
	AlbertosOwner->DissolveBox_Right->ToggleVisibility();
}
#pragma endregion

