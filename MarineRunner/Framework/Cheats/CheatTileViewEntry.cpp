// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Framework/Cheats/CheatTileViewEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

#include "CheatTileViewObject.h"
#include "CheatWidget.h"

void UCheatTileViewEntry::NativeOnInitialized()
{
	CheatButton->OnClicked.AddDynamic(this, &UCheatTileViewEntry::OnCheatButtonClicked);
	CheatButton->OnHovered.AddDynamic(this, &UCheatTileViewEntry::OnCheatButtonHovered);
	CheatButton->OnUnhovered.AddDynamic(this, &UCheatTileViewEntry::OnCheatButtonUnhovered);
}

void UCheatTileViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!IsValid(ListItemObject))
		return;

	TObjectPtr<UCheatTileViewObject> CheatTileViewObject = Cast<UCheatTileViewObject>(ListItemObject);
	if (!IsValid(CheatTileViewObject))
		return;

	CheatDisplayTextBlock->SetText(CheatTileViewObject->EntryCheatData.FunctionDisplayName);
	FunctionNameToCall = CheatTileViewObject->EntryCheatData.FunctionName;
}

void UCheatTileViewEntry::OnCheatButtonClicked()
{
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), FunctionNameToCall);

	PlayAnimationForward(CheatClickAnim);
}

void UCheatTileViewEntry::OnCheatButtonHovered()
{
	PlayAnimationForward(CheatHoverAnim);
}

void UCheatTileViewEntry::OnCheatButtonUnhovered()
{
	PlayAnimationReverse(CheatHoverAnim);
}
