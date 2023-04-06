// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PartyServerManager.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UPartyServerManager : public UObject
{
	GENERATED_BODY()
	
public:
	UPartyServerManager();

	UFUNCTION(BlueprintCallable)
		bool GetPartyList();

};
