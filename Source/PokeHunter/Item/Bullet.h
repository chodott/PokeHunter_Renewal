// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Bullet.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ABullet : public AItem
{
	GENERATED_BODY()
	
public:
	ABullet();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;
	virtual void UseItem(AHunter* ItemOwner) override;
};
