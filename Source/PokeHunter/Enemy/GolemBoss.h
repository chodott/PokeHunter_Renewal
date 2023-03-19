// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GolemBoss.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AGolemBoss : public AEnemy
{
	GENERATED_BODY()

public:
		AGolemBoss();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		TMap<FName, float> PartHP;


public:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//BTTask

	virtual void Attack(int AttackPattern);

	virtual void LongAttack();

	UFUNCTION()
	void LaunchStone();

};
