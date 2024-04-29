// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bullet.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API APotion : public ABullet
{
	GENERATED_BODY()
	
public:

	APotion();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Potion")
	float HealAmount;


public:

	void UseItem_Implementation(class AHunter* ItemOwner)override;
	void UseItem(class AHunter* ItemOwner, FVector InitialPos, FVector EndPos)override;
	void ApplyAbillity(AActor* OtherActor, UPrimitiveComponent* OtherComponent);

	UFUNCTION(NetMulticast, Reliable)
	void MultiAttachPotion(class AHunter* ItemOwner);
	virtual void MultiLaunchBullet_Implementation(class AHunter* BulletOwner, FVector InitialPos, FVector EndPos) override;
};
