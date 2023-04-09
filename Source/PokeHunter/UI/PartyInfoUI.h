// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Network/PartyServerManager.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartyInfoUI.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POKEHUNTER_API UPartyInfoUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UBaseInstance* gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		int SelctPartyNumber = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<FName> PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<PLAYER_PET> PlayerPetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<PLAYER_STATE> PartyMemberState;

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool SendClientState();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool TickSendPartyInfo();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool RecvClientJoin();
};
