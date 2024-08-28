// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlbertosHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UAlbertosHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateSprintBarValue(float NewPercent);

	void ActivateSprintBar(bool bActivate = true);

protected:

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UProgressBar> AlbertosSprintBar = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ActivateSprintBarAnim = nullptr;
};
