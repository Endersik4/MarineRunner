// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"
#include "AIController.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = TEXT("Shoot");
}


EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner()->GetPawn() == nullptr) return EBTNodeResult::Failed;
	
	AEnemyPawn* Enemy = Cast<AEnemyPawn>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (Enemy)
	{
		Enemy->ShootBullet();

		return EBTNodeResult::Succeeded;
	}
	else return EBTNodeResult::Failed;

}
