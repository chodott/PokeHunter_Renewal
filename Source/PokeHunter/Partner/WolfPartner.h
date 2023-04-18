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

	TSubclassOf<class APartnerProjectile> IceShardClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StormCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BreathCollision;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float BreathStartTime{5.0f};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	float BreathTime{ 5.0f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float BreathDamage{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bBreathe{ false };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float StormDamage{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bOnStorm{ false };

protected:
	virtual void Tick(float DeltaTime)override;
	virtual void BeginPlay() override;

	
public:

	void ApplyDamage();
	virtual void UseSpecialSkill(ESkillID SkillID);
	void LaunchIceShard();
	void IceBreathe();
	UFUNCTION(BlueprintCallable)
	void MakeIceShard();
	void MakeStorm();
	void IntoStorm(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OutStorm(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
