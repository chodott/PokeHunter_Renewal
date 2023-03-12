#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"

// #include "Engine.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

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
	UFUNCTION(Blueprintcallable, Category=Socket)
		void ConnectToServer();

};
