// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

#include "CoreMinimal.h"
#include "Net/Subsystems/NetworkSubsystem.h"
#include "InventoryServerManager.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UInventoryServerManager : public UNetworkSubsystem //, public UServerIntance , UInventoryComponent
{
	GENERATED_BODY()
	
public:
	UInventoryServerManager();

	UBaseInstance* gameinstance;

	UFUNCTION(BlueprintCallable)
		bool GetInventoryDBInfos();
};
