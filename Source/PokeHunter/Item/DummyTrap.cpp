// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyTrap.h"
#include "Components/SphereComponent.h"

ADummyTrap::ADummyTrap()
{
	TeamID = FGenericTeamId(0);

	ExplosionCollision->SetGenerateOverlapEvents(false);
}

float ADummyTrap::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HP -= DamageAmount;
	if (HP <= 0.f)	//Explosion
	{
		ExplosionCollision->SetGenerateOverlapEvents(true);
		ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	return DamageAmount;
}

