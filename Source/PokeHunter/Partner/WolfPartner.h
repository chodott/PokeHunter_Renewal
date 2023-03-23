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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:

	virtual void UseSpecialSkill(ESkillID SkillID);
	void LaunchIceShard();
	UFUNCTION(BlueprintCallable)
	void MakeIceShard();
	void MakeStorm();
	void IntoStorm(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OutStorm(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
