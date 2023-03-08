// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyController();

	virtual void OnPossess(APawn* pawn) override;

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
	class UBehaviorTree* BTEnemy;
	UPROPERTY()
	class UBlackboardData* BBEnemy;
};
