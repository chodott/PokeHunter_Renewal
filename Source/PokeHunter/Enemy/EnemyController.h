// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"

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

	virtual void BeginPlay();

	virtual void OnPossess(APawn* pawn) override;

	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
	class UBehaviorTree* BTEnemy;
	UPROPERTY()
	class UBlackboardData* BBEnemy;
	UPROPERTY()
	class UAISenseConfig_Sight* ConfigSight;
	UPROPERTY()
	class UAISenseConfig_Hearing* ConfigHearing;

};
