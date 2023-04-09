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

	Enemy->OnMontageEnd.AddLambda([this]() -> void {
		bPlaying = false;
		});


	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyPlayMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AEnemyController* Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (Controller->FindAgroActor() == true)
	{

		Enemy->CurState = EEnemyState::Roar;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}


	//if (!bPlaying)
	//{
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//}
}
