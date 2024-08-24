// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/Widgets/PlayerAlbertos/AlbertosCommandsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MarineRunner/Player/Components/PlayersAlbertosComponent.h"

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

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->CallAlbertosToPlayer(); };
	}

	AlbertosCommandText->SetText(CallAlbertosText);
}

void UAlbertosCommandsWidget::OnUnhovered_CallAlbertosButton()
{
	PlayAnimationReverse(CallAlbertosButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);
}
#pragma endregion

#pragma region ////// STOP ALBERTOS COMMAND //////
void UAlbertosCommandsWidget::OnHovered_StopButton()
{
	PlayAnimationForward(StopAlbertosButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->StopAlbertosMovement(); };
	}

	AlbertosCommandText->SetText(StopAlbertosText);
}

void UAlbertosCommandsWidget::OnUnhovered_StopButton()
{
	PlayAnimationReverse(StopAlbertosButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);
}
#pragma endregion

#pragma region ////// HACK OBJECT COMMAND //////
void UAlbertosCommandsWidget::OnHovered_HackButton()
{
	PlayAnimationForward(HackButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->HackInteractiveObject(); };
	}

	AlbertosCommandText->SetText(HackInteractiveObjectsText);
}

void UAlbertosCommandsWidget::OnUnhovered_HackButton()
{
	PlayAnimationReverse(HackButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);
}
#pragma endregion

#pragma region ////// OPEN OBJECT COMMAND //////
void UAlbertosCommandsWidget::OnHovered_OpenButton()
{
	PlayAnimationForward(OpenCommandButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->OpenInteractiveObject(); };
	}

	AlbertosCommandText->SetText(OpenInteractiveObjectsText);
}

void UAlbertosCommandsWidget::OnUnhovered_OpenButton()
{
	PlayAnimationReverse(OpenCommandButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);

}
#pragma endregion

#pragma region ////// CHANGE TO ALBERTOS COMMAND //////
void UAlbertosCommandsWidget::OnHovered_ChangeToAlbertosButton()
{
	PlayAnimationForward(ChangeToAlbertosButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->ChangeToAlbertos(); };
	}

	AlbertosCommandText->SetText(ChangeToAlbertosText);
}

void UAlbertosCommandsWidget::OnUnhovered_ChangeToAlbertosButton()
{
	PlayAnimationReverse(ChangeToAlbertosButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);

}
#pragma endregion

#pragma region ////// CRAFT LAST ITEM COMMAND //////
void UAlbertosCommandsWidget::OnHovered_CraftLastItemButton()
{
	PlayAnimationForward(CraftLastItemButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->CraftLastCraftedItem(); };
	}

	AlbertosCommandText->SetText(CraftLastCraftedItemText);
}

void UAlbertosCommandsWidget::OnUnhovered_CraftLastItemButton()
{
	PlayAnimationReverse(CraftLastItemButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);

}
#pragma endregion

#pragma region ////// DAMAGE ENEMY COMMAND //////
void UAlbertosCommandsWidget::OnHovered_DamageEnemyButton()
{
	PlayAnimationForward(DamageEnemyButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->DamageEnemy(); };
	}

	AlbertosCommandText->SetText(DamageEnemyText);

}

void UAlbertosCommandsWidget::OnUnhovered_DamageEnemyButton()
{
	PlayAnimationReverse(DamageEnemyButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);

}
#pragma endregion

#pragma region ////// WALK AROUND COMMAND //////
void UAlbertosCommandsWidget::OnHovered_WalkAroundButton()
{
	PlayAnimationForward(WalkAroundButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = [this]() {this->PlayersAlbertosComponent->StartAlbertosMovement(); };
	}

	AlbertosCommandText->SetText(StartAlbertosMovementText);
}

void UAlbertosCommandsWidget::OnUnhovered_WalkAroundButton()
{
	PlayAnimationReverse(WalkAroundButtonHoverAnim);

	if (IsValid(PlayersAlbertosComponent))
	{
		PlayersAlbertosComponent->AlbertosCommandToExecute = nullptr;
	}

	AlbertosCommandText->SetText(DefaultCommandText);

}
#pragma endregion
