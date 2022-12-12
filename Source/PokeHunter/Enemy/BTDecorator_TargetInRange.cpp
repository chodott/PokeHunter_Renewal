// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_TargetInRange.h"
#include "Enemy.h"
#include "EnemyController.h"

bool UBTDecorator_TargetInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return false;

	if (Enemy->Target == NULL)
		return false;
	
	bResult = (Enemy->Target->GetDistanceTo(Enemy) <= 200);
	return bResult;	
}
