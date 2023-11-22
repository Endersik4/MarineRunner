// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/CallElevatorPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"
#include "MarineRunner/Objects/Elevator/OutsideElevatorDoor.h"


void UCallElevatorPanel::NativeOnInitialized()
{
	CallElevatorButton->OnClicked.AddDynamic(this, &UCallElevatorPanel::OnClickedCallElevatorButton);
	CallElevatorButton->OnHovered.AddDynamic(this, &UCallElevatorPanel::OnHoveredCallElevatorButton);
	CallElevatorButton->OnUnhovered.AddDynamic(this, &UCallElevatorPanel::OnUnhoveredCallElevatorButton);

	WaitForElevatorText->SetVisibility(ESlateVisibility::Hidden);
}

void UCallElevatorPanel::OnClickedCallElevatorButton()
{
	if (IsValid(ElevatorActor) == false)
		return;

	ElevatorActor->GetElevatorPanelWidget()->SelectFloor(Floor);

	CallElevatorAction(ECEA_HideCallAndShowWait);
}

void UCallElevatorPanel::OnHoveredCallElevatorButton()
{
	if (HoverCallElevatorAnim == nullptr)
		return;

	PlayAnimationForward(HoverCallElevatorAnim);
}

void UCallElevatorPanel::OnUnhoveredCallElevatorButton()
{
	if (HoverCallElevatorAnim == nullptr)
		return;

	PlayAnimationReverse(HoverCallElevatorAnim);
}

void UCallElevatorPanel::ShowWaitText(bool bShow)
{
	if (bShow)
	{
		if (AppearWaitForElevatorAnim)
			PlayAnimationForward(AppearWaitForElevatorAnim);
	}
	else
	{
		if (DisappearWaitForElevatorAnim)
			PlayAnimationForward(DisappearWaitForElevatorAnim);
	}
}

void UCallElevatorPanel::ShowCallElevatorPanel(bool bShow)
{
	if (bShow)
	{
		if (AppearCallElevatorAnim)
			PlayAnimationForward(AppearCallElevatorAnim);
	}
	else
	{
		if (DisappearCallElevatorAnim)
			PlayAnimationForward(DisappearCallElevatorAnim);
	}
}

void UCallElevatorPanel::CallElevatorAction(const ECallElevatorAction ActionToDo)
{
	switch (ActionToDo)
	{
		case ECallElevatorAction::ECEA_ShowCall:
		{
			ShowCallElevatorPanel(true);
		}break;
		case ECallElevatorAction::ECEA_HideCallAndShowWait: 
		{
			ShowCallElevatorPanel(false);
			ShowWaitText(true);
		} break;
		case ECallElevatorAction::ECEA_HideCall:
		{
			ShowCallElevatorPanel(false);
		} break;
		case ECallElevatorAction::ECEA_ShowWaitEffect:
		{
			ShowWaitText(true);
		}break;
		case ECallElevatorAction::ECEA_HideWaitEffect:
		{
			ShowWaitText(false);
		}break;
		default: break;
	}
}

bool UCallElevatorPanel::CanCallElevator() const
{
	return CallElevatorButton->GetIsEnabled();
}