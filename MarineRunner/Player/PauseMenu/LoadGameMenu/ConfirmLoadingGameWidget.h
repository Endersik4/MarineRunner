// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmLoadingGameWidget.generated.h"


UENUM(BlueprintType)
enum EConfirmType {
	ECT_LoadLastSave, // Player can confirm if want to load level or no
	ECT_QuitGame // Player can confirm if want to quit the game or no
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
		TObjectPtr<class UImage> BackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> ConfirmText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> YesText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UTextBlock> NoText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> YesButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr < UButton> NoButton = nullptr;

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
		TObjectPtr < UWidgetAnimation> YesHoveredAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> NoHoveredAnim = nullptr;

	void SetConfirmType(EConfirmType NewConfirmType) { ConfirmType = NewConfirmType; }
	void SetSlotAndLevelName(const FString& SlotName, const FString& LevelName);
private:

	EConfirmType ConfirmType = ECT_LoadLastSave;

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true);

	void LoadLastSave();
	UPROPERTY(Transient)
		FString SlotNameToLoad = FString("");
	UPROPERTY(Transient)
		FString LevelNameToLoad = FString("");

	void AddThisWidgetToCurrentSpawnedMenuWidgets(bool bShouldDeleteExistingOne);
	void BackToLoadGame(bool bShouldBack = true);
};
