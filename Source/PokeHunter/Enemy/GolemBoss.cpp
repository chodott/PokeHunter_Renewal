// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "EnemyAnimInstance.h"


AGolemBoss::AGolemBoss()
{

	AttackRange = 1000.f;
}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();
}

float AGolemBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);

		FName HitPart = PointDamageEvent.HitInfo.GetComponent()->ComponentTags[0];
		UE_LOG(LogTemp, Warning, TEXT("%s"), *HitPart.ToString());
		if (PartHP.Contains(HitPart)) PartHP.Emplace(HitPart, PartHP[HitPart] - DamageAmount);

		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	}
	return DamageAmount;
}