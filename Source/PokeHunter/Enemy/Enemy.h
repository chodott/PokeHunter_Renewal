// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageDelegate, float, DamageAmount, FVector, HitLoc);

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase UMETA(DisplayName = "Chase"),
	Hit UMETA(DisplayName = "Hit"),
	Grogy UMETA(DisplayName = "Grogy"),
	Die UMETA(DisplayName = "Die"),
	Roar UMETA(DisplayName = "Roar"),
	Attention UMETA(DisplayName = "Attention"),
	NormalAttack UMETA(DisplayName = "NormalAttack"),
	JumpAttack UMETA(DisplayName = "JumpAttack"),
	LongAttack UMETA(DisplayName = "LongAttack")
	
};


UCLASS()
class POKEHUNTER_API AEnemy : public ACharacter, public IGenericTeamAgentInterface, public IItemInteractInterface, public IEnemyInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HP{ 30 };

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
		class AActor* Target;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class AActor* AgroTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWaitingAgro;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
		TSubclassOf <class AInteractActor> DropItemBoxClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
		TArray<FName> DropItemID_Array;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<AActor*> TargetArray;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
		FVector TargetPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UEnemyAnimInstance* EnemyAnim;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
		EEnemyState CurState {
		EEnemyState::Patrol
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf <class AEnemyProjectile> ProjectileClass;

	FOnMontageEndDelegate OnMontageEnd;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FOnDamageDelegate OnDamage;

	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EarthquakeRange = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ReflectDamgeAmount{ 0.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bReflecting{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float StartBindingTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BindingTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bBinding{ false };

	//상태 이상
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SavedDamage{0};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GrogyDamage{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bGrogy{ false };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bPoisoned{ false };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float PoisonedTime{};
	float StartPoisonedTime;
	int PoisonSaveTime{};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bBurning{ false };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BurningTime{};
	float StartBurningTime;
	int BurningSaveTime{};


	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bDied{ false };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	FGenericTeamId GetGenericTeamId()const override;


	//CollisionFunction
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	bool IsJumping();

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(AEnemy* Enemy, FName Section);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(AEnemy* Enemy, FName Section);
	UFUNCTION(Server, Reliable)
	void ServerStartBinding();
	UFUNCTION(NetMulticast, Reliable)
	void MultiStartBinding();
	UFUNCTION(BlueprintCallable)
	void StartBinding();
	UFUNCTION(BlueprintCallable)
	void StartPoison();
	//UFUNCTION(Server, Reliable)
	//void ServerTakeDamage(AEnemy* Enemy, FName Section);
	//UFUNCTION(NetMulticast, Reliable)
	//void MultiTakeDamage(AEnemy* Enemy);


	void SetTarget(AActor* NewTarget) { Target = NewTarget; };
	void SeeNewTarget(AActor* Actor);
	void HearSound(FVector SoundLoc, AActor* AgroTarget);

	UFUNCTION(BlueprintCallable)
	void ChangeTarget();

	UFUNCTION(BlueprintCallable)
	virtual void Attack(int AttackPattern);
	UFUNCTION(BlueprintCallable)
	virtual void LongAttack();
	UFUNCTION(BlueprintCallable)
	virtual void Roar();
	UFUNCTION(BlueprintCallable)
	virtual void Patrol();
	UFUNCTION(BlueprintCallable)
	virtual void JumpAttack();
	UFUNCTION(BlueprintCallable)
	virtual void LaunchToTarget();
	UFUNCTION(BlueprintCallable)
	virtual void Block();
	UFUNCTION(BlueprintCallable)
	virtual void Reflect();
	UFUNCTION(BlueprintCallable)
	virtual void Die();
	UFUNCTION(BlueprintCallable)
	virtual void SpawnItem();
	

	//Animation Function
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Interface
	virtual void InteractIce_Implementation();
	virtual void InteractFire_Implementation(UPrimitiveComponent* HitComponent);
	virtual void InteractBindTrap_Implementation();

	//EnemyInterface
	virtual void InteractEarthquake_Implementation();
	virtual void InteractAttack_Implementation(FVector HitDirection, float Damage);
	virtual void InteractGrabAttack_Implementation();

public:
	bool bFirstMeet{ true };
};
