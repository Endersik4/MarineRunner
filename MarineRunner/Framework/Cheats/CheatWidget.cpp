// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Framework/Cheats/CheatWidget.h"
#include "Components/TileView.h"

#include "CheatTileViewObject.h"

void UCheatWidget::NativeOnInitialized()
{
	if (!IsValid(CheatsEntryObject))
		return;

	for (const FCheatData& CurrentCheat : AllCheats)
	{
		TObjectPtr<UCheatTileViewObject> CheatObject = NewObject<UCheatTileViewObject>(CheatsEntryObject);
		if (!IsValid(CheatObject))
			continue;

		CheatObject->EntryCheatData = CurrentCheat;
		AllCheatsTileView->AddItem(CheatObject);
	}
}
