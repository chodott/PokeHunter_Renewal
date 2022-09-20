// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HunterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UHunterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
	class AHunter* Hunter;
};
