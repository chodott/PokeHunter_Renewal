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

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Battle")
	float Damage{5.f};

	float TimeLimit{5.f};

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void BeginPlay() override;

	UFUNCTION()
	//void OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	void UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos) ;
};
