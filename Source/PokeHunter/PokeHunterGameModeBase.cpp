// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "PokeHunterGameModeBase.h"
#include "Hunter/Hunter.h"
#include "Hunter/HunterController.h"
#include "Item/ItemDatabase.h"

APokeHunterGameModeBase::APokeHunterGameModeBase()
{
	DefaultPawnClass = AHunter::StaticClass();
	// PlayerControllerClass = AHunterController::StaticClass();
}

void APokeHunterGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	ABLOG(Warning, TEXT("PostLogin End"));
}

FItemInfo APokeHunterGameModeBase::FindItem_Implementation(FName ItemID, bool& Success)
{
	Success = false;
	FItemInfo Info;
	if(ItemDatabase == nullptr) return Info;

	for(int i=0; i<ItemDatabase->InfoArray.Num(); ++i)
	{
		if(ItemDatabase->InfoArray[i].ItemID == ItemID)
		{
			Success = true;
			return ItemDatabase->InfoArray[i];
		}
	}

	return Info;
}

