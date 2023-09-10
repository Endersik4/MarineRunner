// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/BehaviourTreeTasks/BTTask_AlbertosNextLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"


UBTTask_AlbertosNextLocation::UBTTask_AlbertosNextLocation()
{
	NodeName = TEXT("Albertos Next Location");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AlbertosNextLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackBoardComp) return EBTNodeResult::Failed;

	FVector NextLocation = CalculateNextLocationNearThePlayer(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation().Z);

	if (BlackBoardComp->IsVectorValueSet(FName(TEXT("PlayerLocation"))))
	{
		NextLocation = BlackBoardComp->GetValueAsVector(FName(TEXT("PlayerLocation")));
		BlackBoardComp->ClearValue(FName(TEXT("PlayerLocation")));
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NextLocation);

	return EBTNodeResult::Type();
}

EBTNodeResult::Type UBTTask_AlbertosNextLocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type();
}

FVector UBTTask_AlbertosNextLocation::CalculateNextLocationNearThePlayer(float Albertos_Z)
{
	FVector Loc = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	Loc.Z = Albertos_Z;
	float NewX = FMath::FRandRange(-2500.f, 2500.f);
	float NewY = FMath::FRandRange(-2500.f, 2500.f);
	Loc += FVector(NewX, NewY, 0.f);
	return Loc;
}
