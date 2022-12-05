// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{

	MovementSpeed = 0.0f;
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{

	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	else
	{
		Enemy->GetVelocity();
		FVector Speed = Enemy->GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = XYspeed.Size();

		Direction = CalculateDirection(Speed, Enemy->GetActorRotation());
	}
}

void UEnemyAnimInstance::PlayCombatMontage(FName Section)
{
	Montage_Play(CombatMontage, 1.0f);
	Montage_JumpToSection(Section, CombatMontage);
}
