// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxComponent.h"
#include "PokeHunter/Enemy/GolemBoss.h"

UHitBoxComponent::UHitBoxComponent()
{
	SetGenerateOverlapEvents(true);
}


bool UHitBoxComponent::TakeDamage(float DamageAmount)
{
	if (bDestroyed) return false;
	PartHP -= DamageAmount;
	if (PartHP <= 0)
	{
		bDestroyed = true;
		return true;
	}
	
	return false;
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

void UHitBoxComponent::SetChild(UHitBoxComponent* ChildBox )
{
	ChildHitbox = ChildBox;
}

void UHitBoxComponent::DestroyPart()
{
	AGolemBoss* Golem = Cast<AGolemBoss>(GetOwner());
	if (Golem)
	{
		if (ChildHitbox)
		{
			ChildHitbox->DestroyPart(Golem);
		}

		Golem->DeleteHitBox(GetAttachSocketName());

		DestroyComponent();
	}
}

void UHitBoxComponent::DestroyPart(AGolemBoss* Golem)
{
	if (ChildHitbox)
	{
		ChildHitbox->DestroyPart(Golem);
	}
	Golem->DeleteHitBox(GetAttachSocketName());

	DestroyComponent();
}
