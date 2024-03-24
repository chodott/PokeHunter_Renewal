// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "GenericTeamAgentInterface.h"
#include "BaseCharacter.generated.h"

#define MAX_HP 100.f

UCLASS()
class POKEHUNTER_API ABaseCharacter : public ACharacter, public IGenericTeamAgentInterface, public IItemInteractInterface, public IEnemyInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();


	//Battle
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Status")
	float HP{ 100.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bInvincible{ false };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float InvincibleTime{ 1.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	float CurInvincibleTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float HealPerSecondAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bDamaged;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bNoCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float NoCollisionTime{ 0.5f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float CurNoCollisionTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void SetHP(float setHP) { HP = setHP; }
	UFUNCTION(BlueprintCallable)
	float GetHP() { return HP; };

	UFUNCTION(Server, Reliable)
	void ServerStartInvincibility();
	UFUNCTION(NetMulticast, Reliable)
	void MultiStartInvincibility();

	//EnemyInterface Function
	virtual void InteractEarthquake_Implementation();
	virtual void InteractAttack_Implementation(FVector HitDirection, float Damage);
	virtual void InteractGrabAttack_Implementation();
	virtual void InteractWideAttack_Implementation(float Damage);

	//ItemInterface Function
	virtual void InteractPotion_Implementation(float HealAmount);
	virtual void InteractHealArea_Implementation();
	virtual void OutHealArea_Implementation();

	//RPC
	UFUNCTION(Server, Reliable)
	void ServerSpawnEffect(class UNiagaraSystem* Niagara, const FVector& SpawnLoc);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnEffect(class UNiagaraSystem* Niagara, const FVector& SpawnLoc);

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(FName Session);
	UFUNCTION(NetMulticast, Reliable)
	virtual void MultiPlayMontage(FName Session);
};
