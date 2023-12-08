// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FrontChestPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UFrontChestPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCurrentChest(class AChestWithItems* NewChest) { ChestActor = NewChest; }
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* PINText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* OpenButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberOneButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberTwoButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberThreeButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberFourButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberFiveButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberSixButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberSevenButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberEightButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* NumberNineButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* DeleteButton;

	UFUNCTION()
		void OpenButtonPressed();
	UFUNCTION()
		void DeleteButtonPressed();

	UFUNCTION()
		void NumberOnePressed();
	UFUNCTION()
		void NumberTwoPressed();
	UFUNCTION()
		void NumberThreePressed();
	UFUNCTION()
		void NumberFourPressed();
	UFUNCTION()
		void NumberFivePressed();
	UFUNCTION()
		void NumberSixPressed();
	UFUNCTION()
		void NumberSevenPressed();
	UFUNCTION()
		void NumberEightPressed();
	UFUNCTION()
		void NumberNinePressed();

	void ResetPinCodeStrings();

	FORCEINLINE void SetUsePin(bool bUse) { bUsePin = bUse; }
private:
	class AChestWithItems* ChestActor;

	bool bUsePin;
	FString PinCodeString;
	FString PinCodeNotVisibleString;
	void PinNumbersClicked(FString Number);
};
