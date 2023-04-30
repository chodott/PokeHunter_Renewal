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
	TMap<FName, class UHitBoxComponent*> HitBoxMap;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* DiveCurve;

	//Bomb Attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	TArray<TSubclassOf<class AEnemyProjectile>> BombClassArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<class AEnemyProjectile*> BombArray;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float MaxBombRange{ 1000.f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	int MaxBombCnt{ 10 };

	//Wide Attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float WideAttackGap{ 600.f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float WideAttackRadiusGap{ 100.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	int WideAttackCnt{ 0 };
	


public:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep = true, const FHitResult& SweepResult = FHitResult(1.f));
	UFUNCTION(BlueprintNativeEvent)
	void DestroyPart(FName PartName);
	void DeleteHitBox(FName PartName);
	virtual void Die();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Life")
	bool isDie = false;
	//BTTask

	virtual void Attack(int AttackPattern);

	virtual void LongAttack();

	UFUNCTION(BlueprintCallable)
	void Earthquake();

	UFUNCTION(BlueprintCallable)
	void LaunchStone();

	UFUNCTION(BlueprintCallable)
	void SpawnBombs();

	UFUNCTION(BlueprintCallable)
	void CheckWideAttack();

	//ItemInteractInterface
	virtual void InteractFire_Implementation(UPrimitiveComponent* HitComponent);

	//Replication
	void MultiApplyDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult);

	// Return Home
	FTimerHandle ReHomeTimerHandle;
	void OpenLevelHome();
	// GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyActor::MyFunction, 2.0f, false);
};
