// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Roar.h"
#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyController.h"

UBTTask_Roar::UBTTask_Roar()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Roar::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (Enemy == NULL)return EBTNodeResult::Failed;
	bPlaying = true;

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});
	Enemy->Roar();
	return EBTNodeResult::Type::InProgress;
}

void UBTTask_Roar::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bPlaying)
	{
		AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
