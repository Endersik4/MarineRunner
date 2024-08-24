// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayersAlbertosComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UPlayersAlbertosComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayersAlbertosComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void OpenAlbertosCommands();
	void CloseAlbertosCommands();

	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);

	FORCEINLINE void SetAlbertosPawn(TObjectPtr<class AAlbertosPawn> NewAlbertos) { AlbertoPawn = NewAlbertos; }

	FORCEINLINE TObjectPtr<class AAlbertosPawn> GetAlbertosPawn() const { return AlbertoPawn; }

private:
	UPROPERTY(EditAnywhere, Category = "Assigned Albertos Settings")
	bool bAlbertosAssigned = true;
	UPROPERTY(EditAnywhere, Category = "Assigned Albertos Settings")
	TObjectPtr<class AAlbertosPawn> AlbertoPawn = nullptr;
	UPROPERTY(EditAnywhere, Category = "Albertos Commands")
	TSubclassOf<class UAlbertosCommandsWidget> AlbertosCommandsWidgetClass = nullptr;

	void SpawnAlbertosCommandsWidget();

	UPROPERTY(Transient)
	TObjectPtr<class UAlbertosCommandsWidget> AlbertosCommandsWidget = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
};
