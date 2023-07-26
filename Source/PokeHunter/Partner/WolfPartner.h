// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Partner.h"
#include "WolfPartner.generated.h"


/**
 * 
 */
UCLASS()
class POKEHUNTER_API AWolfPartner : public APartner
{
	GENERATED_BODY()

public:
	AWolfPartner();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class APartnerProjectile> IceShardClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StormCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BreathCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particle")
	class UNiagaraComponent* IceBreatheEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particle")
	class UNiagaraComponent* IceStormEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particle")
	class UNiagaraSystem* IceHornEffect = nullptr;


	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Skill")
	float BreatheLimitTime{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float BreathTime{ 5.0f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float BreathDamage{};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Skill")
	bool bBreathe{ false };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float StormTime{ 10.f };
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Skill")
	float StormLimitTime{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float StormDamage{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float StormSize{4.f};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Skill")
	bool bOnStorm{ false };

protected:
	virtual void Tick(float DeltaTime)override;
	virtual void BeginPlay() override;

	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	void ApplyDamage();
	virtual void UseSpecialSkill(ESkillID SkillID);
	virtual void CancelOrder();
	void LaunchIceShard();
	UFUNCTION(BlueprintCallable)
	void ActivateBreathe();
	void IceBreathe();
	void ResetBreathe();
	UFUNCTION(BlueprintCallable)
	void MakeIceShard();
	UFUNCTION(BlueprintCallable)
	void ActivateStorm();
	void MakeStorm();
};
