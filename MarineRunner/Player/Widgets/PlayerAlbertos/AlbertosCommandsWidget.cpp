// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/Widgets/PlayerAlbertos/AlbertosCommandsWidget.h"
#include "Components/Button.h"
void UAlbertosCommandsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CallAlbertosCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_CallAlbertosButton);
	CallAlbertosCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_CallAlbertosButton);

	StopAlbertosCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_StopButton);
	StopAlbertosCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_StopButton);

	HackCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_HackButton);
	HackCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_HackButton);

	OpenCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_OpenButton);
	OpenCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_OpenButton);

	ChangeToAlbertosCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_ChangeToAlbertosButton);
	ChangeToAlbertosCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_ChangeToAlbertosButton);

	CraftLastItemCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_CraftLastItemButton);
	CraftLastItemCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_CraftLastItemButton);

	DamageEnemyCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_DamageEnemyButton);
	DamageEnemyCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_DamageEnemyButton);

	WalkAroundCommandButton->OnHovered.AddDynamic(this, &UAlbertosCommandsWidget::OnHovered_WalkAroundButton);
	WalkAroundCommandButton->OnUnhovered.AddDynamic(this, &UAlbertosCommandsWidget::OnUnhovered_WalkAroundButton);

	PlayAnimationForward(ShowAlbertosCommandsAnim);
}

#pragma region ////// CALL ALBERTOS COMMAND //////
void UAlbertosCommandsWidget::OnHovered_CallAlbertosButton()
{
	PlayAnimationForward(CallAlbertosButtonHoverAnim);
}

void UAlbertosCommandsWidget::OnUnhovered_CallAlbertosButton()
{
	PlayAnimationReverse(CallAlbertosButtonHoverAnim);
}
#pragma endregion

#pragma region ////// STOP ALBERTOS COMMAND //////
void UAlbertosCommandsWidget::OnHovered_StopButton()
{
	PlayAnimationForward(StopAlbertosButtonHoverAnim);

}

void UAlbertosCommandsWidget::OnUnhovered_StopButton()
{
	PlayAnimationReverse(StopAlbertosButtonHoverAnim);
}
#pragma endregion

#pragma region ////// HACK OBJECT COMMAND //////
void UAlbertosCommandsWidget::OnHovered_HackButton()
{
	PlayAnimationForward(HackButtonHoverAnim);

}

void UAlbertosCommandsWidget::OnUnhovered_HackButton()
{
	PlayAnimationReverse(HackButtonHoverAnim);
}
#pragma endregion

#pragma region ////// OPEN OBJECT COMMAND //////
void UAlbertosCommandsWidget::OnHovered_OpenButton()
{
	PlayAnimationForward(OpenCommandButtonHoverAnim);

}

void UAlbertosCommandsWidget::OnUnhovered_OpenButton()
{
	PlayAnimationReverse(OpenCommandButtonHoverAnim);
}
#pragma endregion

#pragma region ////// CHANGE TO ALBERTOS COMMAND //////
void UAlbertosCommandsWidget::OnHovered_ChangeToAlbertosButton()
{
	PlayAnimationForward(ChangeToAlbertosButtonHoverAnim);
}

void UAlbertosCommandsWidget::OnUnhovered_ChangeToAlbertosButton()
{
	PlayAnimationReverse(ChangeToAlbertosButtonHoverAnim);
}
#pragma endregion

#pragma region ////// CRAFT LAST ITEM COMMAND //////
void UAlbertosCommandsWidget::OnHovered_CraftLastItemButton()
{
	PlayAnimationForward(CraftLastItemButtonHoverAnim);

}

void UAlbertosCommandsWidget::OnUnhovered_CraftLastItemButton()
{
	PlayAnimationReverse(CraftLastItemButtonHoverAnim);
}
#pragma endregion

#pragma region ////// DAMAGE ENEMY COMMAND //////
void UAlbertosCommandsWidget::OnHovered_DamageEnemyButton()
{
	PlayAnimationForward(DamageEnemyButtonHoverAnim);

}

void UAlbertosCommandsWidget::OnUnhovered_DamageEnemyButton()
{
	PlayAnimationReverse(DamageEnemyButtonHoverAnim);
}
#pragma endregion

#pragma region ////// WALK AROUND COMMAND //////
void UAlbertosCommandsWidget::OnHovered_WalkAroundButton()
{
	PlayAnimationForward(WalkAroundButtonHoverAnim);
}
void UAlbertosCommandsWidget::OnUnhovered_WalkAroundButton()
{
	PlayAnimationReverse(WalkAroundButtonHoverAnim);
}
#pragma endregion
