// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PokeHunterStateBase.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API APokeHunterStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated)
		FString LatestEvent;

	UPROPERTY(Replicated)
		FString WinningTeam;

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
