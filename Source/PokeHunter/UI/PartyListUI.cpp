// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyListUI.h"

bool UPartyListUI::GetPartyList()
{
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
	if (0 == gameinstance->PartyListMap.Num()) return false;

	

	return true;
}
