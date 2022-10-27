// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractActor.h"
#include "PokeHunter/Item/ItemData.h"
#include "ItemDropActor.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AItemDropActor : public AInteractActor
{
	GENERATED_BODY()

public:

	AItemDropActor();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	TMap<int32,TSubclassOf<UItemData>> ItemDataClassMap;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Probability")
		int32 DropCnt {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Probability")
		int32 DropItemCnt {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Probability")
		int32 BaseProbability {};


	virtual void Interact_Implementation(AHunter* Hunter) override;
};
