// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class POKEHUNTER_API IItemInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractIce();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractFire(UPrimitiveComponent* HitComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractPotion(float HealAmount);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractBindTrap();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractHealArea();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OutHealArea();



};
