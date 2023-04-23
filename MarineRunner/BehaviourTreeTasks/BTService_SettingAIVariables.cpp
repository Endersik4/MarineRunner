// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SettingAIVariables.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/EnemiesClasses/EnemyAiController.h"

UBTService_SettingAIVariables::UBTService_SettingAIVariables()
{
	NodeName = "Update Player Location If seen (AI Variables)";
	bNotifyTick = true;
}

void UBTService_SettingAIVariables::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	SetPlayerLocation(OwnerComp);
}

void UBTService_SettingAIVariables::SetPlayerLocation(UBehaviorTreeComponent& OwnerComp)
{
	AEnemyAiController* EnemyAIController = Cast<AEnemyAiController>(OwnerComp.GetAIOwner());
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!EnemyAIController || !PlayerPawn) return;

	if (EnemyAIController->GetDoEnemySeePlayer() == true)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	}
}
