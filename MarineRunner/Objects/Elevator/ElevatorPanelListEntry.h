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

	virtual void NativeOnInitialized() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* SelectFloorButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* FloorTextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* HoverSelectFloor = nullptr;

	UFUNCTION()
		void OnClickedSelectFloorButton();
	UFUNCTION()
		void OnHoveredSelectFloorButton();
	UFUNCTION()
		void OnUnhoveredSelectFloorButton();

	void DisableElevatorPanelEntry(bool bDisable = true);
private:
	class USelectFloorEntryObject* EntryFloor;

};

