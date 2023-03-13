#pragma once

#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"

// #include "Engine.h"

#include "MySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UMySubsystem : public USubsystem
{
	GENERATED_BODY()
	
public:
	UMySubsystem();

	FSocket* Socket;

	FString address;
	int32 port;
	FIPv4Address ip;

	UFUNCTION(BlueprintCallable, Category=Socket)
		void ConnectToServer();

};
