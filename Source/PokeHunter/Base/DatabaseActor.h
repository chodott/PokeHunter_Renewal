// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeHunter/Item/ItemData.h"
#include "DatabaseActor.generated.h"

UCLASS()
class POKEHUNTER_API ADatabaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADatabaseActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly,category = "ItemData")
	TMap<FName,TSubclassOf<class UItemData>> ItemDataClassMap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,category = "ItemData")
	TMap<FName, class UItemData*> ItemDataObjectMap;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	UItemData* FindItem(FName ItemID);

};
