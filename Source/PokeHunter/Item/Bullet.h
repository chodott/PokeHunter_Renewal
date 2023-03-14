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

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Battle")
	float Damage{};

	float TimeLimit{5.f};

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	void UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos) ;
};
