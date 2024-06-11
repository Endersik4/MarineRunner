// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "CheatTileViewEntry.generated.h"

UCLASS()
class MARINERUNNER_API UCheatTileViewEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> CheatButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> CheatDisplayTextBlock = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CheatClickAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CheatHoverAnim = nullptr;

	UFUNCTION()
	void OnCheatButtonClicked();
	UFUNCTION()
	void OnCheatButtonHovered();
	UFUNCTION()
	void OnCheatButtonUnhovered();

	UPROPERTY(Transient)
	FString FunctionNameToCall = FString();
};
