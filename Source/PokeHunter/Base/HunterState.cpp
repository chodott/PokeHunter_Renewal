// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterState.h"
#include "Net/UnrealNetwork.h"

void AHunterState::InitPlayerData()
{
	totalDamage = 0.0f;
}

void AHunterState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHunterState, Team);
}

FU_DamageInfo AHunterState::getTotalDamage()
{
	AHunterController* hunterContoller = Cast<AHunterController>(GetOwner());

	int32 hunterDamage = Cast<AHunter>(hunterContoller->GetCharacter())->bTotalDamaged;
	int32 partnerDamage = Cast<AHunter>(hunterContoller->GetCharacter())->bPartnerTotalDamaged;

	return FU_DamageInfo{ hunterDamage, partnerDamage };
}
