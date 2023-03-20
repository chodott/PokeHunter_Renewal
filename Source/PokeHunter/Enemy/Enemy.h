// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase UMETA(DisplayName = "Chase"),
	Hit UMETA(DisplayName = "Hit"),
	Die UMETA(DisplayName = "Die"),
	Roar UMETA(DisplayName = "Roar"),
	Attention
	
};


UCLASS()
class POKEHUNTER_API AEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float HP{30};

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
	class APawn* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class APawn*> TargetArray;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
	FVector TargetPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEnemyAnimInstance* EnemyAnim;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
	EEnemyState CurState{EEnemyState::Patrol};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf <class AEnemyProjectile> ProjectileClass;

	FOnMontageEndDelegate OnMontageEnd;

	//TeamID
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 200.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	//CollisionFunction
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	bool IsJumping();

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(AEnemy* Enemy, FName Section);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(AEnemy* Enemy, FName Section);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	void SeeNewTarget(AActor* Actor);
	void HearSound(FVector SoundLoc);

	UFUNCTION(BlueprintCallable)
	virtual void Attack(int AttackPattern);
	UFUNCTION(BlueprintCallable)
	virtual void LongAttack();
	UFUNCTION(BlueprintCallable)
	virtual void Roar();
	UFUNCTION()
	virtual void Patrol();
	virtual void JumpAttack();

	//Animation Function
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	bool bFirstHit{ true};
	bool bFirstMeet{ true };
};
