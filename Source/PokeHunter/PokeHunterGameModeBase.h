// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PokeHunter.h"
#include "GameFramework/GameModeBase.h"
#include "Item/ItemDatabase.h"
#include "PokeHunterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API APokeHunterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	APokeHunterGameModeBase();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	//Item Database
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Database")
	class UItemDatabase* ItemDatabase;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ItemMgr")
	FItemInfo FindItem(FName ItemID, bool& Success);
	FItemInfo FindItem_Implementation(FName ItemID, bool& Success);
};
