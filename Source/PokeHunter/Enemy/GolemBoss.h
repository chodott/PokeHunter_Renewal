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

	//HitBox
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	TArray<class UHitBoxComponent*> PartHitBoxs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* HeadHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* BodyHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* LeftArmHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* LeftHandHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* LeftLegHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* RightShoulderHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* RightArmHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* RightHandHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* RightLegHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBox")
	class ACharacter* GrabbedTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBox")
	bool bCanGrab;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range")
	class UStaticMeshComponent* EarthquakeCollision;



public:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep = true, const FHitResult& SweepResult = FHitResult(1.f));
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintNativeEvent)
	void DestroyPart(FName PartName);

	//BTTask

	virtual void Attack(int AttackPattern);

	virtual void LongAttack();

	UFUNCTION(BlueprintCallable)
	void Earthquake();

	UFUNCTION(BlueprintCallable)
	void LaunchStone();

	//ItemInteractInterface
	virtual void InteractFire_Implementation(UPrimitiveComponent* HitComponent);

};
