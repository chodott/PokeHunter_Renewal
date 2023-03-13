// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../../PH-Server/IOCPServer/protocol.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"
#include "Net/Subsystems/NetworkSubsystem.h"
#include "MyNetworkSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FU_SC_LOGIN_INFO_PACK {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _player_skin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _pet_num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString q_item;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString q_skill;
};

UCLASS()
class POKEHUNTER_API UMyNetworkSubsystem : public UNetworkSubsystem
{
	GENERATED_BODY()
	
public:
	UMyNetworkSubsystem();

	SOCKET Socket;
	WSADATA wsaData;
	SOCKADDR_IN stServerAddr;

	int reval;

	UFUNCTION(BlueprintCallable, Category = Socket)
		FU_SC_LOGIN_INFO_PACK ConnectToServer(FString in_id, FString in_pw);
};
