// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlbertosCommandsWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
UCLASS()
class MARINERUNNER_API UAlbertosCommandsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TObjectPtr<class UPlayersAlbertosComponent> PlayersAlbertosComponent = nullptr;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AlbertosCommandText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowAlbertosCommandsAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> CallAlbertosCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> CallAlbertosCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CallAlbertosButtonHoverAnim = nullptr;	
	UFUNCTION()
	void OnHovered_CallAlbertosButton();
	UFUNCTION()
	void OnUnhovered_CallAlbertosButton();


	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> StopAlbertosCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> StopAlbertosCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StopAlbertosButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_StopButton();
	UFUNCTION()
	void OnUnhovered_StopButton();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> HackCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> HackCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HackButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_HackButton();
	UFUNCTION()
	void OnUnhovered_HackButton();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> OpenCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> OpenCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OpenCommandButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_OpenButton();
	UFUNCTION()
	void OnUnhovered_OpenButton();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> ChangeToAlbertosCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage>  ChangeToAlbertosCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ChangeToAlbertosButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_ChangeToAlbertosButton();
	UFUNCTION()
	void OnUnhovered_ChangeToAlbertosButton();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> CraftLastItemCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> CraftLastItemCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CraftLastItemButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_CraftLastItemButton();
	UFUNCTION()
	void OnUnhovered_CraftLastItemButton();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> DamageEnemyCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> DamageEnemyCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageEnemyButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_DamageEnemyButton();
	UFUNCTION()
	void OnUnhovered_DamageEnemyButton();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> WalkAroundCommandButton = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> WalkAroundCommandImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WalkAroundButtonHoverAnim = nullptr;
	UFUNCTION()
	void OnHovered_WalkAroundButton();
	UFUNCTION()
	void OnUnhovered_WalkAroundButton();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText DefaultCommandText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText CallAlbertosText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText StopAlbertosText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText HackInteractiveObjectsText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText OpenInteractiveObjectsText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText ChangeToAlbertosText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText CraftLastCraftedItemText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText DamageEnemyText = FText();
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Commands Names")
	FText StartAlbertosMovementText = FText();
};
