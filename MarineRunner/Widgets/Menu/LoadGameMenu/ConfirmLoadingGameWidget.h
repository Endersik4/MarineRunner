// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmLoadingGameWidget.generated.h"


UENUM(BlueprintType)
enum EConfirmType {
	ECT_LoadLastSave,
	ECT_QuitGame
};
class UTextBlock;
class UButton;
UCLASS()
class MARINERUNNER_API UConfirmLoadingGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ConfirmText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* YesText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* NoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* YesButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* NoButton;

	UFUNCTION()
		void YesButton_OnClicked();
	UFUNCTION()
		void YesButton_OnHovered();
	UFUNCTION()
		void YesButton_OnUnhovered();

	UFUNCTION()
		void NoButton_OnClicked();
	UFUNCTION()
		void NoButton_OnHovered();
	UFUNCTION()
		void NoButton_OnUnhovered();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* YesHoveredAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* NoHoveredAnim = nullptr;

	void SetConfirmType(EConfirmType NewConfirmType) { ConfirmType = NewConfirmType; }
	void SetSlotAndLevelName(const FString& SlotName, const FString& LevelName);
private:

	EConfirmType ConfirmType = ECT_LoadLastSave;

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true);

	void LoadLastSave();
	FString SlotNameToLoad;
	FString LevelNameToLoad;

	void AddThisWidgetToCurrentSpawnedMenuWidgets(bool bShouldDeleteExistingOne);
	void BackToLoadGame(bool bShouldBack = true);
};
