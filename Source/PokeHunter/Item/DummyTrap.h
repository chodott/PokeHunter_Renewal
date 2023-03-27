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

public:
	//TeamID
	FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HP;


};
