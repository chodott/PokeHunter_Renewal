// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "LobbyLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ALobbyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "ItemData")
	TArray<TSubclassOf<class UItemData>> ItemDataClassArray;
	UPROPERTY(EditAnywhere, Category = "ItemData")
	TArray<class UItemData*> ItemDataObjectArray;

protected:
	virtual void BeginPlay() override;
};
