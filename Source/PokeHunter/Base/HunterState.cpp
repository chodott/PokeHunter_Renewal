// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterState.h"
#include "Net/UnrealNetwork.h"

void AHunterState::InitPlayerData()
{
	totalDamage = 0.0f;
	
	/*auto instance = Cast<UBaseInstance>(GetGameInstance());

	instance->SendAccessToken();

	HunterSkin = instance->mySkin;

	UE_LOG(LogTemp, Warning, TEXT("[HunterSkin] : %d\n[instance->mySkin] : %d"), HunterSkin, instance->mySkin);*/
}

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
