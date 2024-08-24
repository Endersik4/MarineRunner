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
#include "MarineRunner/Albertos/AlbertosAIController.h"

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

	if (IsValid(AlbertoPawn))
	{
		AlbertosAIController = Cast<AAlbertosAIController>(AlbertoPawn->GetController());
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
	AlbertosCommandsWidget->PlayersAlbertosComponent = this;
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

	if (AlbertosCommandToExecute)
	{
		AlbertosCommandToExecute();
		AlbertosCommandToExecute = nullptr;
	}
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

#pragma endregion 

#pragma region ////////// COMMANDS //////////
void UPlayersAlbertosComponent::CallAlbertosToPlayer()
{
	if (!IsValid(AlbertoPawn) || !IsValid(Player))
		return;

	if (!IsValid(AlbertoPawn->GetAlbertosToPlayerComponent()) || Player->GetIsInCutscene())
		return;

	AlbertoPawn->GetAlbertosToPlayerComponent()->CallAlbertosToThePlayer(Player->GetActorLocation());
}

void UPlayersAlbertosComponent::StopAlbertosMovement()
{
	if (!IsValid(AlbertosAIController))
		return;

	AlbertosAIController->SetCanMove(false);
	AlbertosAIController->StopMovement();
}

void UPlayersAlbertosComponent::HackInteractiveObject()
{
	UE_LOG(LogTemp, Warning, TEXT("3"));

}

void UPlayersAlbertosComponent::OpenInteractiveObject()
{
	UE_LOG(LogTemp, Warning, TEXT("4"));

}

void UPlayersAlbertosComponent::ChangeToAlbertos()
{
	UE_LOG(LogTemp, Warning, TEXT("5"));

}

void UPlayersAlbertosComponent::CraftLastCraftedItem()
{
	UE_LOG(LogTemp, Warning, TEXT("6"));

}

void UPlayersAlbertosComponent::DamageEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("7"));

}

void UPlayersAlbertosComponent::StartAlbertosMovement()
{
	if (!IsValid(AlbertosAIController))
		return;

	AlbertosAIController->SetCanMove(true);
}
#pragma endregion