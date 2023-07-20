// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/PokeHunter.h"
#include "GameFramework/PlayerController.h"
#include "HunterController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AHunterController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "HunterSkin")
		int32 SelectedMaterialIndex;

	UBaseInstance* baseinstance;*/

public:
	// AHunterController();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	//Replicated
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//void SelectMaterial(int32 SelectedMaterialIndex);

	//// UFUNCTION(Server, Reliable, WithValidation)
	//UFUNCTION(Server, WithValidation)
	//	void Server_SendMaterialInfo(int32 SelectedMaterialIndex);
};
