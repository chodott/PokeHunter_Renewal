// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyPlayMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyController.h"

UBTTask_EnemyPlayMontage::UBTTask_EnemyPlayMontage()
{
	NodeName = TEXT("PlayMontage");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_EnemyPlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return EBTNodeResult::Failed;

	bPlaying = true;

	//Patrol
	Enemy->Patrol();

	switch (Enemy->CurState)
	{
	case EEnemyState::LeftDestroy:
		Enemy->ServerPlayMontage(Enemy, FName("LeftDestroy"));
		break;
	case EEnemyState::RightDestroy:
		Enemy->ServerPlayMontage(Enemy, FName("RightDestroy"));
		break;
	}

	Enemy->OnMontageEnd.AddLambda([this]() -> void {
		bPlaying = false;
		});


	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyPlayMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AEnemyController* Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	//Patrol 종료 조건
	//if (Controller->FindAgroActor() == true)
	//{

	//	Enemy->CurState = EEnemyState::Chase;
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//}


	if (!bPlaying)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		Enemy->CurState = EEnemyState::Chase;
	}
}
