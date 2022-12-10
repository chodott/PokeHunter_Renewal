// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PartnerAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class POKEHUNTER_API UPartnerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPartnerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
		void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
		float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
		class APartner* Partner;

};
