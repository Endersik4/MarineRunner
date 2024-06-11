// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CheatWidget.generated.h"

USTRUCT(BlueprintType)
struct FCheatData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
	FText FunctionDisplayName = FText();
	UPROPERTY(EditDefaultsOnly)
	FString FunctionName = FString();
};

UCLASS()
class MARINERUNNER_API UCheatWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTileView> AllCheatsTileView = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Cheats Settings")
	TArray<FCheatData> AllCheats;
	UPROPERTY(EditDefaultsOnly, Category = "Cheats Settings")
	TSubclassOf<class UCheatTileViewObject> CheatsEntryObject = nullptr;

};
