// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Network/PartyServerManager.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartyListUI.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UPartyListUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UBaseInstance* gameinstance;

	UFUNCTION(BlueprintCallable, Category = "Party")
		bool GetPartyList();
};
