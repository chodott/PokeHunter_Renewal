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

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (Enemy == NULL) return EBTNodeResult::Failed;
	if (EnemyAnim == NULL) return EBTNodeResult::Failed;

	bPlaying = true;

	switch (Enemy->CurState)
	{
	case EEnemyState::Hit:
		EnemyAnim->bPlaying = false;
		EnemyAnim->PlayCombatMontage("Hit");
		break;

	case EEnemyState::Roar:
		EnemyAnim->PlayCombatMontage("Roar");
		break;

	case EEnemyState::Die:
		EnemyAnim->bPlaying = false;
		EnemyAnim->PlayCombatMontage("Die");
		break;
		
	case EEnemyState::Patrol:
		EnemyAnim->PlayCombatMontage("Patrol");
		break;

	default:
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

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (!bPlaying)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}