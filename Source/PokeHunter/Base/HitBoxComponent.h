// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "HitBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UHitBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Part")
	float PartHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Part")
	bool bDestroyed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	float StartBurningTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	float SaveBurningTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	float BurningTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	bool bBurning;

	

public:
	UHitBoxComponent();
	void TakeDamage(float DamageAmount);
	bool CheckBurning(float DeltaTime);

};
