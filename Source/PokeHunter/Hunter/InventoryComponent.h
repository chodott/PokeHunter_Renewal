// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PokeHunter/Item/ItemData.h"
#include "InventoryComponent.generated.h"



UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class POKEHUNTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemCnter> InfoArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemCnter> FarmingArray;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int capacity = 24;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool AddItemData(FItemCnter ItemCnter);

	UFUNCTION(BlueprintCallable)
	void ChangeSlot(UInventoryComponent* otherInventory, int thisIndex, int otherIndex);
	UFUNCTION(BlueprintCallable)
	int GetItemCnt(FName id);
	UFUNCTION(BlueprintCallable)
	bool CreateItem(const FItemInfo& ItemInfo);
	
};
