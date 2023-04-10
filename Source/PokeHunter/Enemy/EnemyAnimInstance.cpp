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
		bJumping = Enemy->IsJumping();

		Direction = CalculateDirection(Speed, Enemy->GetActorRotation());
	}
}

void UEnemyAnimInstance::StopCombatMontage(float BlendTime)
{
	Montage_Stop(BlendTime, CombatMontage);
}

bool UEnemyAnimInstance::PlayCombatMontage(FName Section, bool bInterrupt)
{
	//if (Section == FName("Die") || Section == FName("Hit"))
	if(bInterrupt)
	{
		
		if (Section == FName("Patrol"))
		{
			float StartTime = FMath::RandRange(13.f,20.f);
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_SetPosition(CombatMontage, StartTime);
		}
		else
		{
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_JumpToSection(Section, CombatMontage);
		}
		

		return true;
	}
	else if (!bPlaying)
	{
		if (Section == FName("Patrol"))
		{
			float StartTime = FMath::RandRange(13.f,20.f);
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_SetPosition(CombatMontage, StartTime);
		}

		else
		{
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_JumpToSection(Section, CombatMontage);
		}
		return true;
	}
	return false;
}
