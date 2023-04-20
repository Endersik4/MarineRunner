// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "EnemyPawn.h"

void AEnemyAiController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehaviour != nullptr)
	{
		RunBehaviorTree(AIBehaviour); //Odpala dany BehaviourTree ktory
		//ustawia sie w UPROPERTY()

		//Root moze miec tylko jedno "dziecko"
		//Sequance to sekwencja zadan ktore beda wykonywac sie po kolei
		//Czyli Najpierw MoveTo, Wait, MoveTo, Wait.


		// Selector sprawdza od lewej do prawej czy dana rzecz sie moze wykonac jezeli 
		// CanSeePlayer? bedzie false to wykona sie Invistigate. Cos jak Switch()
		// Decorator powoduje że Sequance Node zmienia swoj sposob wykonania
		// Bez Decoratora sequence sie wykona niezaleznie od niczego.
		// Mozna dodawac wiele Decoratorow
		// CanSeePlayer? To Decorator Blackboard sprawdza czy Vector PlayerLocation 
		// jest ustawiony i jezeli jest to wykona sie CHASE
		// ConeCheck Decorator sprawdza kąt wzroku czy Gracz w nim się znajduje

		AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());

		if (EnemyPawn)
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), EnemyPawn->GetActorLocation());

			GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), EnemyPawn->GetHowManyLocations());
			GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), EnemyPawn->GetHowManyLocations());
		}
		//Ustawiasz Wartosc w Blackboard, i zeby ta wartosc byla w Blackboard w Edytorze to trzeba tam dodac
		//taka sama zmienna, wszystko sie musi zgadzac. Mozna tez inne wartosci UStawic np. SetValueAsFloat itd
	}

	//APawn* MarinePawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	//SetFocus(MarinePawn);
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	//MoveToActor(PlayerPawn, 500);

	if (LineOfSightTo(PlayerPawn)) //zwraca bool czy dany aktor jest
		//w lini wzroku
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
	}
	else
	{
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation")); //Czysci wartość, uzyteczne w Decorator->Blackboard
	}*/
}
