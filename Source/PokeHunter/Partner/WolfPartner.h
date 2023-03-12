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

	class UCapsuleComponent* StormCollision;
	
public:
	virtual void UseSpecialSkill(ESkillID SkillID);
	void LaunchIceShard();
	void MakeStorm();
};
