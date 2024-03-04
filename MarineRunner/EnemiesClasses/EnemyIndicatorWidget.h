// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyIndicatorWidget.generated.h"

class UProgressBar;
UCLASS()
class MARINERUNNER_API UEnemyIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> CurrentHealthBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> DelayedHealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> EnemyIndicatorImage;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DelayedHealthTime = 0.5f;

	void SetCurrentHealthInHealthBar(float NewHealth);

	FORCEINLINE void SetMaxHealth(float _MaxHealth) { MaxHealth = _MaxHealth; }
private:
	float MaxHealth;

	FTimerHandle DelayedHealthProgressBarHandle;
	void ApplyCurrentHealthToDelayedBar();

};
