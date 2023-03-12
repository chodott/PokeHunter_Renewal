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
		UMySubsystem();
	
public:
	FSocket* Socket;

	FString address;
	int32 port;
	FIPv4Address ip;

	UFUNCTION(Blueprintcallable, Category=Socket)
		void ConnectToServer();

};
