// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "PinNumberTileEntry.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API UPinNumberTileEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> PinNumberButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PinNumberText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverPinNumberButton = nullptr;

	UFUNCTION()
	void OnClickedPinNumberButton();
	UFUNCTION()
	void OnHoveredPinNumberButton();
	UFUNCTION()
	void OnUnhoveredPinNumberButton();

private:

	UPROPERTY(Transient)
	TObjectPtr<class UPinNumberEntryObject> PinNumberEntryObject = nullptr;
};
