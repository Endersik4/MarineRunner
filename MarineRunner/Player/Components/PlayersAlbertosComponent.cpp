// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/Components/PlayersAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "MarineRunner/Player/Widgets/PlayerAlbertos/AlbertosCommandsWidget.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/Components/AlbertosToPlayerComponent.h"
#include "MarineRunner/Albertos/Widgets/Crafting/CraftingAlbertosWidget.h"

UPlayersAlbertosComponent::UPlayersAlbertosComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayersAlbertosComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GetOwner()))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
}

void UPlayersAlbertosComponent::OpenAlbertosCommands()
{
	if (!bAlbertosAssigned)
		return;

	if (!IsValid(AlbertosCommandsWidgetClass) || !IsValid(PlayerController))
		return;

	SpawnAlbertosCommandsWidget();

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, AlbertosCommandsWidget);
	PlayerController->SetShowMouseCursor(true);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	PlayerController->SetMouseLocation(ViewportSize.X/2.f, ViewportSize.Y/2.f);
}

void UPlayersAlbertosComponent::SpawnAlbertosCommandsWidget()
{
	TObjectPtr<UUserWidget> TempSpawnedWidget = CreateWidget(PlayerController, AlbertosCommandsWidgetClass);
	if (!IsValid(TempSpawnedWidget))
		return;

	AlbertosCommandsWidget = Cast<UAlbertosCommandsWidget>(TempSpawnedWidget);
	if (!IsValid(AlbertosCommandsWidget))
		return;

	AlbertosCommandsWidget->AddToViewport();
}

void UPlayersAlbertosComponent::CloseAlbertosCommands()
{
	if (!bAlbertosAssigned)
		return;

	if (IsValid(AlbertosCommandsWidget))
	{
		AlbertosCommandsWidget->SetVisibility(ESlateVisibility::Collapsed);
		AlbertosCommandsWidget->RemoveFromParent();
		AlbertosCommandsWidget = nullptr;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	PlayerController->SetShowMouseCursor(false);
}

#pragma region //////////////////////////////// ALBERTO ////////////////////////////////
void UPlayersAlbertosComponent::UpdateAlbertosInventory(bool bShouldUpdateInventory, bool bShouldUpdateCrafting)
{
	if (!bAlbertosAssigned)
		return;

	if (!IsValid(AlbertoPawn) || !IsValid(Player))
		return;

	TObjectPtr<UCraftingAlbertosWidget> CraftingWidget = Cast<UCraftingAlbertosWidget>(AlbertoPawn->GetCraftingTableWidget());
	if (!IsValid(CraftingWidget))
		return;

	if (bShouldUpdateInventory)
	{
		CraftingWidget->AddItemsToInventoryTileView(Player->GetInventoryComponent()->Inventory_Items);
	}

	if (bShouldUpdateCrafting)
	{
		Player->GetInventoryComponent()->MoveWeaponRecipesToEndQueue();
		CraftingWidget->SetPlayer(Player);
		CraftingWidget->SetRecipesData(Player->GetInventoryComponent()->Items_Recipes);
		CraftingWidget->SwitchCurrentCraftingItem();
	}
}

/*
void UPlayersAlbertosComponent::CallAlbertosPressed()
{
	if (!IsValid(AlbertoPawn) || Player->GetIsInCutscene())
		return;

	if (!IsValid(AlbertoPawn->GetAlbertosToPlayerComponent()))
		return;

	//AlbertoPawn->GetAlbertosToPlayerComponent()->CallAlbertosToThePlayer(GetActorLocation());
}*/
#pragma endregion 
