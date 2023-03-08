// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyLevelScriptActor.h"
#include "Item/ItemData.h"

void ALobbyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	for (auto& ItemDataClass : ItemDataClassArray)
	{
		auto ItemData = NewObject<UItemData>(GetWorld(), ItemDataClass);
		ItemDataObjectArray.Add(ItemData);
	}
}
