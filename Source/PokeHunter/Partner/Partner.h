// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "PokeHunter/Base/SkillData.h"
#include "PokeHunter/Base/PartnerSkillInterface.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Base/BaseCharacter.h"
#include "Partner.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);


UENUM(BlueprintType)
enum class EPartnerState : uint8
{
	//Stay State
	Posing,
	Unselected,
	MoveTarget,
	Knockback,

	//Leave State
	Rushing,
	SlashAttack,
	Howling,
	IceShard,
	MakingStorm,
	IceBreath
};

UCLASS()
class POKEHUNTER_API APartner : public ABaseCharacter , public IPartnerSkillInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EPartnerType Type = EPartnerType::NonePartner;

	// Sets default values for this character's properties
	APartner();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	class AHunter* Hunter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPartnerAnimInstance* PartnerAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* curMontage;
	
	FOnMontageEndDelegate OnMontageEnd;

	UBaseInstance* baseinstace;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
	FVector TargetPos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector LookTargetVec;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector AttackPoint;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	class ACharacter* Target;
	
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	EPartnerState CurState;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	bool bPosing;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	bool bOrdered;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	bool bUsingSkill;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI");
	bool bDied;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bGrabbed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float SlashDamage{ 10 };

	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FGenericTeamId TeamID;

	//Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* SlashHitEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* SlashMoveEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	float MoveEffectOffsetX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//Perception
	FGenericTeamId GetGenericTeamId()const override;

	UFUNCTION(BlueprintCallable)
	inline EPartnerType GetType() { return Type; }

	// Status
	UFUNCTION(BlueprintCallable)
		void ResetStatus(FVector startPos) {
		StopAnimMontage(curMontage); 
		HP = MAX_HP; CurState = EPartnerState::MoveTarget;
		TargetPos = startPos;
		bDied = false;
	}

	UFUNCTION(BlueprintCallable)
	bool CheckFalling();

	virtual void SlashAttack();
	UFUNCTION(BlueprintCallable)
	virtual void SlashMove();
	virtual void Howling();
	virtual void StopSkill();
	virtual void UseSkill(ESkillID SkillID);
	virtual void CancelOrder();
	void SetTarget(ACharacter* setTarget);
	void FollowHunter(class AHunter* Hunter);
	void SetPosition(const FVector& LocVec);

	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

	//Replication
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerApplyDamage(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MultiApplyDamage(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser);

	virtual void MultiPlayMontage_Implementation(FName Section) ;
	UFUNCTION(Server, Reliable)
	void ServerStopMontage(APartner* Partner);
	UFUNCTION(NetMulticast, Reliable)
	void MultiStopMontage(APartner* Partner);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetPosition(const FVector& LocVec);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetHunter(class AHunter* OwnerHunter);
	UFUNCTION(NetMulticast, Reliable)
	void MultiUseSkill(ESkillID SkillID);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCancelOrder();

};
