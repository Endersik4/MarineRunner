// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Door/DoorWidgets/DoorPanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MarineRunner/Objects/Door/Door.h"


void UDoorPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InteractDoorButton->OnClicked.AddDynamic(this, &UDoorPanelWidget::OnClickedInteractDoorButton);
	InteractDoorButton->OnHovered.AddDynamic(this, &UDoorPanelWidget::OnHoveredInteractDoorButton);
	InteractDoorButton->OnUnhovered.AddDynamic(this, &UDoorPanelWidget::OnUnhoveredInteractDoorButton);
}

void UDoorPanelWidget::OnClickedInteractDoorButton()
{
	if (IsValid(DoorActor) == false)
		return;

	if (bDoorOpen == false)
	{
		DoorActor->OpenDoor();
		InteractDoorText->SetText(CloseDoorText);
		bDoorOpen = true;
	}
	else
	{
		DoorActor->CloseDoor();
		InteractDoorText->SetText(OpenDoorText);
		bDoorOpen = false;
	}

	PlayAnimationForward(OnClickedInteractDoorAnim);
}

void UDoorPanelWidget::OnHoveredInteractDoorButton()
{
	if (HoverInteractDoorAnim == nullptr)
		return;

	PlayAnimationForward(HoverInteractDoorAnim);
}

void UDoorPanelWidget::OnUnhoveredInteractDoorButton()
{
	if (HoverInteractDoorAnim == nullptr)
		return;

	PlayAnimationReverse(HoverInteractDoorAnim);
}