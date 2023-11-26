// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UDoorPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FORCEINLINE void SetDoorActor(class ADoor* NewDoorActor) const { DoorActor = NewDoorActor; }
private:

	class ADoor* DoorActor;
};
