// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Elevator/CallElevatorPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "MarineRunner/Objects/Elevator/Elevator.h"
#include "MarineRunner/Objects/Elevator/ElevatorPanelWidget.h"

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

	if (DisappearCallElevatorAnim)
		PlayAnimationForward(DisappearCallElevatorAnim);

	if (AppearWaitForElevatorAnim)
		PlayAnimationForward(AppearWaitForElevatorAnim);
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

void UCallElevatorPanel::ActiveCallElevatorPanel()
{
	//if (AppearCallElevatorAnim)
	//	PlayAnimationForward(AppearCallElevatorAnim);

	if (ActiveWaitForElevatorAnim)
		StopAnimation(ActiveWaitForElevatorAnim);

	if (DisappearWaitForElevatorAnim)
		PlayAnimationForward(DisappearWaitForElevatorAnim);
}
