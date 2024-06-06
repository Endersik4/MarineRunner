// Copyright Adam Bartela.All Rights Reserved


#include "ConfirmOptionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "GameMenuBase.h"

void UConfirmOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmButton->OnClicked.AddDynamic(this, &UConfirmOptionWidget::ConfirmButton_OnClicked);
	ConfirmButton->OnHovered.AddDynamic(this, &UConfirmOptionWidget::ConfirmButton_OnHovered);
	ConfirmButton->OnUnhovered.AddDynamic(this, &UConfirmOptionWidget::ConfirmButton_OnUnhovered);

	DenyButton->OnClicked.AddDynamic(this, &UConfirmOptionWidget::DenyButton_OnClicked);
	DenyButton->OnHovered.AddDynamic(this, &UConfirmOptionWidget::DenyButton_OnHovered);
	DenyButton->OnUnhovered.AddDynamic(this, &UConfirmOptionWidget::DenyButton_OnUnhovered);
}

void UConfirmOptionWidget::AddThisWidgetToCurrentSpawnedMenuWidgets(bool bRemoveWidget)
{
	if (!IsValid(CurrentSpawnedMenu))
		return;

	if (bRemoveWidget)
	{
		CurrentSpawnedMenu->CurrentSpawnedMenuWidgets.Pop();
	}
	else
	{
		CurrentSpawnedMenu->CurrentSpawnedMenuWidgets.Add(FVisiblePauseMenu(this, [this](bool b) { this->RemoveConfirmWidget(b); }));
	}
}

void UConfirmOptionWidget::RemoveConfirmWidget(bool bRemove)
{
	if (!bRemove)
		return;

	AddThisWidgetToCurrentSpawnedMenuWidgets(true);
	RemoveFromParent();
}

#pragma region //////////// Confirm BUTTON ////////////////
void UConfirmOptionWidget::ConfirmButton_OnClicked()
{
	ConfirmFunction();

	RemoveConfirmWidget(true);
}

void UConfirmOptionWidget::ConfirmButton_OnHovered()
{
	PlayAnimationForward(ConfirmButtonHoveredAnim);
}

void UConfirmOptionWidget::ConfirmButton_OnUnhovered()
{
	PlayAnimationReverse(ConfirmButtonHoveredAnim);
}
#pragma endregion

#pragma region ////////////// Deny BUTTON /////////////
void UConfirmOptionWidget::DenyButton_OnClicked()
{
	AddThisWidgetToCurrentSpawnedMenuWidgets(true);
	RemoveFromParent();
}

void UConfirmOptionWidget::DenyButton_OnHovered()
{
	PlayAnimationForward(DenyButtonHoveredAnim);
}

void UConfirmOptionWidget::DenyButton_OnUnhovered()
{
	PlayAnimationReverse(DenyButtonHoveredAnim);
}
#pragma endregion
