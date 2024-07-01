// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthIndicatorWidget.generated.h"

class UProgressBar;
UCLASS()
class MARINERUNNER_API UEnemyIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> CurrentHealthBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> DelayedHealthBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> EnemyIndicatorImage = nullptr;

public:
	void SetCurrentHealthInHealthBar(const float NewHealth);
	void RestartEnemyHealthBar();

	FORCEINLINE void SetMaxHealth(float _MaxHealth) { MaxHealth = _MaxHealth; }
private:
	UPROPERTY(EditAnywhere)
	float DelayedHealthTime = 0.5f;
	UPROPERTY(Transient)
	float MaxHealth = 0.f;

	FTimerHandle DelayedHealthProgressBarHandle;
	void ApplyCurrentHealthToDelayedBar();

};
