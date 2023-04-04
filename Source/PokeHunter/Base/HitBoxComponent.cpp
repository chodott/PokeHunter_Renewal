// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxComponent.h"

UHitBoxComponent::UHitBoxComponent()
{
	SetGenerateOverlapEvents(true);
}

void UHitBoxComponent::TakeDamage(float DamageAmount)
{
	PartHP -= DamageAmount;
	if (PartHP <= 0)
	{
		bDestroyed = true;
	}
}

bool UHitBoxComponent::CheckBurning(float DeltaTime)
{
	if (!bBurning) return false;
	float ElapsedTime = GetWorld()->TimeSeconds - StartBurningTime;
	int CurSecond = FMath::FloorToInt(ElapsedTime);
	float TimeLeft = BurningTime - ElapsedTime;
	if (TimeLeft <= 0.0f)
	{
		bBurning = false;
		CurSecond = 0;
		return false;
	}
	else
	{
		if (CurSecond == SaveBurningTime)
		{
			return false;
		}
		else
		{
			SaveBurningTime = CurSecond;
			return true;
		}
	}

}
