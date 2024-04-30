// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmOptionWidget.generated.h"

class UTextBlock;
class UButton;
UCLASS()
class MARINERUNNER_API UConfirmOptionWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> BackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> DisplayInformationText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> ConfirmText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> DenyText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> ConfirmButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> DenyButton = nullptr;

	UFUNCTION()
	void ConfirmButton_OnClicked();
	UFUNCTION()
	void ConfirmButton_OnHovered();
	UFUNCTION()
	void ConfirmButton_OnUnhovered();

	UFUNCTION()
	void DenyButton_OnClicked();
	UFUNCTION()
	void DenyButton_OnHovered();
	UFUNCTION()
	void DenyButton_OnUnhovered();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> ConfirmButtonHoveredAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> DenyButtonHoveredAnim = nullptr;

	// When player confirms then this function is called
	TFunction<void()> ConfirmFunction;

private:

	void AddThisWidgetToCurrentSpawnedMenuWidgets(bool bRemoveWidget);
	void RemoveConfirmWidget(bool bRemove = true);
};
