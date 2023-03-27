// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*
#include "../../../../PH-Server/IOCPServer/protocol.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
*/

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "BaseInstance.generated.h"

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
class POKEHUNTER_API UBaseInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UBaseInstance();

	/*
	SOCKET Socket;
	WSADATA wsaData;
	SOCKADDR_IN stServerAddr;

	int reval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Player_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Player_Skin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Pet_Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Quick_Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Quick_Skill;

	UFUNCTION(BlueprintCallable, Category = Socket)
		bool ConnectToServer();
	*/
	
	virtual void Shutdown() override;

	UPROPERTY()
		FString AccessToken;

	UPROPERTY()
		FString IdToken;

	UPROPERTY()
		FString RefreshToken;

	UPROPERTY()
		FTimerHandle RetrieveNewTokensHandle;

	UFUNCTION()
		void SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken);

private:
	FHttpModule* HttpModule;

	UPROPERTY()
		FString ApiUrl;

	UFUNCTION()
		void RetrieveNewTokens();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
