// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxComponent.h"
#include "PokeHunter/Enemy/GolemBoss.h"
#include "Net/UnrealNetwork.h"

UHitBoxComponent::UHitBoxComponent()
{
	SetGenerateOverlapEvents(true);
	SetIsReplicatedByDefault(true);
}

void UHitBoxComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHitBoxComponent, PartHP);
	DOREPLIFETIME(UHitBoxComponent, bDestroyed);

}


void UHitBoxComponent::TakeDamage(float DamageAmount)
{
	if (bDestroyed) return;
	PartHP -= DamageAmount;
	if (PartHP <= 0)
	{
		bDestroyed = true;
		AGolemBoss* Golem = Cast<AGolemBoss>(GetOwner());
		//BlueprintNative Event ȣ��
		ServerDestroyPart(Golem);
		FString partName = GetAttachSocketName().ToString();
		if (partName.Contains("Left"))
		{	//���� ���� �ı�
			Golem->DestroyLeftArm();
		}
		else
		{	//������ �����ı�
			Golem->DestroyRightArm();
		}
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

void UHitBoxComponent::SetChild(UHitBoxComponent* ChildBox )
{
	ChildHitbox = ChildBox;
}

void UHitBoxComponent::ServerDestroyPart_Implementation(AGolemBoss* Golem)
{
	MultiDestroyPart(Golem);
}

void UHitBoxComponent::MultiDestroyPart_Implementation(AGolemBoss* Golem)
{
	DestroyPart(Golem);
}

void UHitBoxComponent::DestroyPart(AGolemBoss* Golem)
{
	if (ChildHitbox)
	{
		ChildHitbox->DestroyPart(Golem);
	}

	Golem->DeleteHitBox(GetAttachSocketName());
	Golem->DestroyPart(GetAttachSocketName());
	DestroyComponent();
}