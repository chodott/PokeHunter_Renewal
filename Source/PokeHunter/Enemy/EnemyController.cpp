// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "EnemyController.h"

AEnemyController::AEnemyController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Enemy/BB_Enemy.BB_Enemy"));
	if (BBObject.Succeeded())
	{
		BBEnemy = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Enemy/BT_Enemy.BT_Enemy"));
	if (BTObject.Succeeded())
	{
		BTEnemy = BTObject.Object;
	}
}

void AEnemyController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
	RunAI();
}

void AEnemyController::RunAI()
{
	//upgrade5
	//if (UseBlackboard(BBEnemy, Blackboard)) RunBehaviorTree(BTEnemy);
	RunBehaviorTree(BTEnemy);
}

void AEnemyController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent == nullptr) return;
	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}
