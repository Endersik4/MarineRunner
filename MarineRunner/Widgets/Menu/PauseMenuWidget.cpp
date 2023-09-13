// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedResumeButton);
	ResumeButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredResumeButton);
	ResumeButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredResumeButton);

	LoadGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedLoadGameButton);
	LoadGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredLoadGameButton);
	LoadGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredLoadGameButton);

	SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedSettingsButton);
	SettingsButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredSettingsButton);
	SettingsButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredSettingsButton);

	RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedRestartButton);
	RestartButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredRestartButton);
	RestartButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredRestartButton);

	QuitGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnClickedQuitGameButton);
	QuitGameButton->OnHovered.AddDynamic(this, &UPauseMenuWidget::OnHoveredQuitGameButton);
	QuitGameButton->OnUnhovered.AddDynamic(this, &UPauseMenuWidget::OnUnhoveredQuitGameButton);
}

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

#pragma region //////////////// RESUME /////////////////
void UPauseMenuWidget::OnClickedResumeButton()
{

}

void UPauseMenuWidget::OnHoveredResumeButton()
{
	ResumeText->SetColorAndOpacity(TextOnHoverColor);
}

void UPauseMenuWidget::OnUnhoveredResumeButton()
{
	ResumeText->SetColorAndOpacity(TextOriginalColor);

}
#pragma endregion 

#pragma region //////////////// LOAD GAME /////////////////
void UPauseMenuWidget::OnClickedLoadGameButton()
{
}

void UPauseMenuWidget::OnHoveredLoadGameButton()
{
	LoadGameText->SetColorAndOpacity(TextOnHoverColor);
}

void UPauseMenuWidget::OnUnhoveredLoadGameButton()
{
	LoadGameText->SetColorAndOpacity(TextOriginalColor);
}
#pragma endregion 

#pragma region //////////////// SETTINGS /////////////////
void UPauseMenuWidget::OnClickedSettingsButton()
{

}

void UPauseMenuWidget::OnHoveredSettingsButton()
{
	SettingsText->SetColorAndOpacity(TextOriginalColor);
}

void UPauseMenuWidget::OnUnhoveredSettingsButton()
{
	SettingsText->SetColorAndOpacity(TextOriginalColor);
}

#pragma endregion 

#pragma region //////////////// RESTART /////////////////
void UPauseMenuWidget::OnClickedRestartButton()
{
}

void UPauseMenuWidget::OnHoveredRestartButton()
{
	RestartText->SetColorAndOpacity(TextOnHoverColor);
}

void UPauseMenuWidget::OnUnhoveredRestartButton()
{
	RestartText->SetColorAndOpacity(TextOriginalColor);
}
#pragma endregion 

#pragma region //////////////// QUIT GAME /////////////////
void UPauseMenuWidget::OnClickedQuitGameButton()
{
}

void UPauseMenuWidget::OnHoveredQuitGameButton()
{
	QuitGameText->SetColorAndOpacity(TextOnHoverColor);
}

void UPauseMenuWidget::OnUnhoveredQuitGameButton()
{
	QuitGameText->SetColorAndOpacity(TextOriginalColor);
}
#pragma endregion 
