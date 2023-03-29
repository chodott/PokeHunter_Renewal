// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "PokeHunter/Base/HitBoxComponent.h"
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
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
		TArray<class UHitBoxComponent*> PartHitBox;
		/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
		class UHitBoxComponent* BodyHitBox;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
		class UHitBoxComponent* LeftArmHitBox;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
		class UHitBoxComponent* LeftLegHitBox;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
		class UHitBoxComponent* RightArmHitBox;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
		class UHitBoxComponent* RightLegHitBox;*/


public:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//BTTask

	virtual void Attack(int AttackPattern);

	virtual void LongAttack();

	UFUNCTION(BlueprintCallable)
	void LaunchStone();

	//ItemInteractInterface
	virtual void InteractFire_Implementation(UPrimitiveComponent* HitComponent);

};
