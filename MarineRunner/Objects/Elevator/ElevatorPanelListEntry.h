// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "ElevatorPanelListEntry.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UElevatorPanelListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> SelectFloorButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> FloorTextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> HoverSelectFloor = nullptr;

	UFUNCTION()
		void OnClickedSelectFloorButton();
	UFUNCTION()
		void OnHoveredSelectFloorButton();
	UFUNCTION()
		void OnUnhoveredSelectFloorButton();

private:
	void DisableElevatorPanelEntry(bool bDisable = true);

	UPROPERTY(Transient)
		TObjectPtr<class USelectFloorEntryObject> FloorEntryObject;
	void SetUpElevatorEntry();
};

