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
		class UButton* PinNumberButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* PinNumberText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* HoverPinNumberButton = nullptr;

	UFUNCTION()
		void OnClickedPinNumberButton();
	UFUNCTION()
		void OnHoveredPinNumberButton();
	UFUNCTION()
		void OnUnhoveredPinNumberButton();

private:

	class UPinNumberEntryObject* PinNumberEntryObject;
	
};
