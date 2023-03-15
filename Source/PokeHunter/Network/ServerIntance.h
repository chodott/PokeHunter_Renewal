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
#include "Engine/GameInstance.h"
#include "ServerIntance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FU_SC_LOGIN_INFO_PACK {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _player_skin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _pet_num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _q_item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _q_skill;
};

USTRUCT(BlueprintType)
struct FU_SC_ITEM_INFO_PACK {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName _name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int cnt;
};

UCLASS()
class POKEHUNTER_API UServerIntance : public UGameInstance
{
	GENERATED_BODY()
public:
	UServerIntance();

	SOCKET Socket;
	WSADATA wsaData;
	SOCKADDR_IN stServerAddr;

	int reval;

	UFUNCTION(BlueprintCallable, Category = Socket)
		BOOL ConnectToServer();

	UFUNCTION(BlueprintCallable, Category = Socket)
		void USendPacket(void* packet);
};
