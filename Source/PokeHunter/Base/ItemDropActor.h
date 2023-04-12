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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	TArray<FItemCnter> DropItemArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	TMap<int32,FName> DropItemMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	TMap<int32,FName> ItemInfoMap;

	FVector TurningPointVec;
	FVector StartPointVec;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TotalTime{ 1.f };
	float RunningTime;
	bool bInteracting{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Probability")
		int32 DropCnt {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Probability")
		int32 DropItemCnt {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Probability")
		int32 BaseProbability {};

public:
	AItemDropActor();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void CreateItemArray(TArray<FName> &ItemArray);
	FVector CalculatePoint(float DeltaTime);

	virtual void Interact_Implementation(AHunter* Hunter) override;
};
