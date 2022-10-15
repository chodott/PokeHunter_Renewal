// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include "PokeHunter/IOnlineSession.h"
// #include "Interfaces/OnlineSessionInterface.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"
// #include "../../../../UnrealEngine-4.27-plus/Engine/Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UDefaultGameInstance();

	//FOnCreateSessionCompleteDelegate CreateSessionCompleteD;
	//FDelegateHandle CreateSessionCompleteDH;
};
