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

