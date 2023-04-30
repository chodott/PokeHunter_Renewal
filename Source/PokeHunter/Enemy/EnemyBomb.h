// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyProjectile.h"
#include "EnemyBomb.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AEnemyBomb : public AEnemyProjectile
{
	GENERATED_BODY()
	
public:
	AEnemyBomb();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void DropBomb();

	virtual void InteractChargeAttack_Implementation();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpSpeed{150.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GravityValue{10.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bActive{ false };
};
