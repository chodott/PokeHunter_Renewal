// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/HunterController.h"
#include "HunterState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FU_DamageInfo {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 hunterAmount {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 petAmount {};
};

UCLASS()
class POKEHUNTER_API AHunterState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY()
		FString PlayerSessionId;

	UPROPERTY()
		FString MatchmakingPlayerId;

	UPROPERTY(Replicated)
		FString Team;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
		float totalDamage;

public:
	void InitPlayerData();

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	UFUNCTION(BlueprintCallable, Category = "Battle")
		FU_DamageInfo getTotalDamage();
};
