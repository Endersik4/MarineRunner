// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

#include "EnemyPawn.h"

// Zeby Stworzyc customowy Task do Behaviour Tree trzeba w AllClasses
// znalezc BTTask (mozna inne rodzaje) i do build.cs trzeba dodac "GameplayTasks"
// Jest to Customowy Task ktory ma wartosc od BlackBoard
// Po stworzeniu customowego Taska wystarczy w Behaviour Tree prawym przyciskiem myszy go dodac
// podajac nazwe z NodeName
UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue()
{	
	// Nazwa Taska
	NodeName = TEXT("Clear Blackboard Value");}
// Funkcja Wykonujaca (protected)
EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey()); //Biore wartosc od Blackboard
	AEnemyPawn* Enemy = Cast<AEnemyPawn>(OwnerComp.GetAIOwner()->GetPawn()); //GetAIOwner() adres Enemy

	return EBTNodeResult::Succeeded; //zwraca czy Task sie powiodl, jest jeszcze InProgress, Failed
}
