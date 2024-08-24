// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HackMiniGameWidget.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class MARINERUNNER_API UHackMiniGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UProgressBar> TimeLeftProgressBar = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstPinNumber = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondPinNumber = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ThirdPinNumber = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> FourPinNumber = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UButton> ChooseNumberButton = nullptr;

	UFUNCTION()
	void OnClicked_ChooseNumberButton();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Hack Mini Game Settings")
	float MaxTimePlay = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hack Mini Game Settings")
	float NormalNumberVisibleTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Hack Mini Game Settings")
	float VisibleCorrectNumberTime = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Hack Mini Game Settings")
	FFloatRange RandomTimeToShowCorrectNumber = FFloatRange(0.5f, 4.f);
	UPROPERTY(EditDefaultsOnly, Category = "Hack Mini Game Settings")
	FSlateColor OriginalTextColor = FSlateColor();
	UPROPERTY(EditDefaultsOnly, Category = "Hack Mini Game Settings")
	FSlateColor CorrectNumberTextColor = FSlateColor();

	UPROPERTY(Transient)
	bool bGameStarted = false;
	UPROPERTY(Transient)
	int32 CurrentIndexOfPin = 0;
	UPROPERTY(Transient)
	int32 CurrentCorrectNumber = 0;
	UPROPERTY(Transient)
	FString CorrectPin = "4819";
	UPROPERTY(Transient)
	FTimerHandle ShowCorrectNumberHandle = FTimerHandle();
	void ShowCorrectNumber();
	UPROPERTY(Transient)
	bool bCorrectNumberVisible = false;

	UPROPERTY(Transient)
	int32 LastRandomNumber = 0;

	UPROPERTY(Transient)
	FTimerHandle UpdateCurrentNumberHandle = FTimerHandle();
	void UpdateCurrentNumber();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> AllPinTextBlocks;
};
