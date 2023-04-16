// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"
#include "PokeHunter/Hunter/Hunter.h"

APotion::APotion()
{
	ItemType = EItemType::Potion;
}

void APotion::UseItem_Implementation(APawn* ItemOwner)
{
	AHunter* Hunter = Cast<AHunter>(ItemOwner);
	if (Hunter) Hunter->HunterInfo.HunterHP += HealAmount;
}

