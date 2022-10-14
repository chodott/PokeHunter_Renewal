// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\PokeHunter.h"
#include "GameFramework/PlayerController.h"
#include "HunterController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AHunterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;
};
