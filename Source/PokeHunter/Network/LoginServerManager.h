// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ServerIntance.h"

#include "CoreMinimal.h"
#include "Net/Subsystems/NetworkSubsystem.h"
#include "LoginServerManager.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ULoginServerManager : public UNetworkSubsystem, public UServerIntance
{
	GENERATED_BODY()
public:
	ULoginServerManager();

	UFUNCTION(BlueprintCallable, Category = Socket)
		FU_SC_LOGIN_INFO_PACK LoginToServer(FString in_id, FString in_pw);
};
