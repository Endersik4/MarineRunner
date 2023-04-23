// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SettingAIVariables.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/EnemiesClasses/EnemyAiController.h"

UBTService_SettingAIVariables::UBTService_SettingAIVariables()
{
	NodeName = "Update Player Location If seen";
}

void UBTService_SettingAIVariables::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyAiController* EnemyAIController = Cast<AEnemyAiController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!EnemyAIController || BlackBoardComp == nullptr || PlayerPawn == nullptr) return;

	if (EnemyAIController->GetDoEnemySeePlayer() == true)
	{
		BlackBoardComp->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	}
}
