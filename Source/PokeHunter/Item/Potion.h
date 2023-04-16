// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API APotion : public AItem
{
	GENERATED_BODY()
	
public:

	APotion();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Potion")
	float HealAmount;


public:

	void UseItem_Implementation(APawn* ItemOwner)override;
};
