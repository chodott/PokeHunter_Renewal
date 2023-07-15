// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterState.h"
#include "Net/UnrealNetwork.h"

void AHunterState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHunterState, Team);
}

float AHunterState::getTotalDamage()
{
	AHunterController* hunterContoller = Cast<AHunterController>(GetOwner());
	return Cast<AHunter>(hunterContoller->GetCharacter())->bTotalDamaged;
}
