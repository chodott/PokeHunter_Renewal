// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

#include "CoreMinimal.h"
#include "Net/Subsystems/NetworkSubsystem.h"
#include "MyNetworkSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UMyNetworkSubsystem : public UNetworkSubsystem
{
	GENERATED_BODY()
	
public:
	UMyNetworkSubsystem();

	FSocket* Socket;

	FString address;
	int32 port;
	FIPv4Address ip;

	UFUNCTION(BlueprintCallable, Category = Socket)
		void ConnectToServer();

};
