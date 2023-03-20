// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"


AGolemBoss::AGolemBoss()
{

	AttackRange = 1000.f;
}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();
}

void AGolemBoss::LongAttack()
{
	if (EnemyAnim)
	{
		MultiPlayMontage(this, FName("Throw"));
	}
	TargetPos = Target->GetActorLocation();
}

void AGolemBoss::LaunchStone()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand"));
	if (Target == NULL) UE_LOG(LogTemp, Warning, TEXT("Dd"));
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - InitialPos;
	DirectionVec.Normalize();


	AEnemyProjectile* Stone = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, InitialPos, DirectionVec.Rotation());
	Stone->FireInDirection(DirectionVec, InitialPos, EndPos);
}

float AGolemBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);

		FName HitPart = PointDamageEvent.HitInfo.GetComponent()->ComponentTags[0];
		UE_LOG(LogTemp, Warning, TEXT("%s"), *HitPart.ToString());
		if (PartHP.Contains(HitPart)) PartHP.Emplace(HitPart, PartHP[HitPart] - DamageAmount);

	}
	return DamageAmount;
}

void AGolemBoss::Attack(int AttackPattern)
{
	switch (AttackPattern)
	{
	case 0:
		if (Target != NULL)
		{
			ServerPlayMontage(this, FName("Attack"));
			//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
		}
		break;
	case 1:
		ServerPlayMontage(this, FName("Attack_Punch"));
		break;

	}
}