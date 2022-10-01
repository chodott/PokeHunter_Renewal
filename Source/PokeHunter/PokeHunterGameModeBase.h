// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PokeHunter.h"
#include "GameFramework/GameModeBase.h"
#include "PokeHunterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API APokeHunterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	APokeHunterGameModeBase();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
