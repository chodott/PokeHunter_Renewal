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

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* HeadCollisionBox;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* BodyCollisionBox;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* LeftArmCollisionBox;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* RightArmCollisionBox;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent*	LeftLegCollisionBox;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent*	RightLegCollisionBox;

public:
	virtual void BeginPlay() override;

};
