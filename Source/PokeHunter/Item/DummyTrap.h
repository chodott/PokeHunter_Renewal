// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplosionTrap.h"
#include "GenericTeamAgentInterface.h"
#include "DummyTrap.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ADummyTrap : public AExplosionTrap, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ADummyTrap();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	//TeamID
	FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HP;


};
