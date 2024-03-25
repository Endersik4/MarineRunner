// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Components/CraftingWidgetAnimationComponent.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"


UCraftingWidgetAnimationComponent::UCraftingWidgetAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCraftingWidgetAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Albertos Pawn is nullptr in CraftItemAlbertosComponent!")))
	{
		AlbertosOwner = Cast<AAlbertosPawn>(GetOwner());
	}
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
	if (!bIsCraftingWidgetAnimatinPlaying)
		return;
	if (!IsValid(AlbertosOwner))
		return;

	if (CraftingWidgetAnimationTimeElapsed >= CraftingWidgetAnimationTime)
	{
		CraftingWidgetAnimationEnded();
	}

	const FVector& NewBoxLeftLoc = FMath::Lerp(DissolveBoxStartLoc_L, DissolveBoxEndLoc_L, CraftingWidgetAnimationTimeElapsed / CraftingWidgetAnimationTime);
	const FVector& NewBoxRightLoc = FMath::Lerp(DissolveBoxStartLoc_R, DissolveBoxEndLoc_R, CraftingWidgetAnimationTimeElapsed / CraftingWidgetAnimationTime);

	AlbertosOwner->DissolveBox_Left->SetRelativeLocation(NewBoxLeftLoc);
	AlbertosOwner->DissolveBox_Right->SetRelativeLocation(NewBoxRightLoc);

	CraftingWidgetAnimationTimeElapsed += Delta;
}

void UCraftingWidgetAnimationComponent::CraftingWidgetAnimationEnded()
{
	if (!IsValid(AlbertosOwner))
		return;

	ToggleVisibilityForDissolveBoxes();
	bIsCraftingWidgetAnimatinPlaying = false;

	if (!bWasCraftingWidgetClosed)
		return;
	
	AlbertosOwner->ToggleVisibilityCraftingWidget();
	bWasCraftingWidgetClosed = false;
}

void UCraftingWidgetAnimationComponent::PrepareCraftingWidgetAnimation(bool bForwardAnim)
{
	if (!IsValid(AlbertosOwner))
		return;

	bIsCraftingWidgetAnimatinPlaying = true;

	DissolveBoxStartLoc_L = AlbertosOwner->DissolveBox_Left->GetRelativeLocation();
	DissolveBoxStartLoc_R = AlbertosOwner->DissolveBox_Right->GetRelativeLocation();

	CraftingWidgetAnimationTimeElapsed = 0.f;

	if (bForwardAnim)
	{
		DissolveBoxEndLoc_L = DissolveBoxStartLoc_L + DissolveBoxesOffsetForAnim;
		DissolveBoxEndLoc_R = DissolveBoxStartLoc_R - DissolveBoxesOffsetForAnim;
	}
	else
	{
		DissolveBoxEndLoc_L = DissolveBoxStartLoc_L - DissolveBoxesOffsetForAnim;
		DissolveBoxEndLoc_R = DissolveBoxStartLoc_R + DissolveBoxesOffsetForAnim;
		bWasCraftingWidgetClosed = true;
	}

	ToggleVisibilityForDissolveBoxes();
}

void UCraftingWidgetAnimationComponent::ToggleVisibilityForDissolveBoxes()
{
	if (!IsValid(AlbertosOwner))
		return;

	AlbertosOwner->DissolveBox_Left->ToggleVisibility();
	AlbertosOwner->DissolveBox_Right->ToggleVisibility();
}
#pragma endregion

