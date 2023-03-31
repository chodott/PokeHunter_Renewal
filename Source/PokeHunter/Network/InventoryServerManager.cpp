 // Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "InventoryServerManager.h"

UInventoryServerManager::UInventoryServerManager()
{
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

bool UInventoryServerManager::GetInventoryDBInfos()
{
	TArray<AActor*> foundDBActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADatabaseActor::StaticClass(), foundDBActors);
	ADatabaseActor* itemDB = Cast<ADatabaseActor>(foundDBActors[0]);

	// Database 연결 전 BP_DatabaseActor 의 값을 사용함.
	for (auto& ItemDataClass : itemDB->ItemDataClassMap)
	{
		auto ItemData = NewObject<UItemData>(this, ItemDataClass.Value);
		itemDB->ItemDataObjectMap.Add(ItemDataClass.Key, ItemData);
	}
	return true;

	CS_QUEST_INVENTORY_PACK quest_item;
	quest_item.size = sizeof(CS_QUEST_INVENTORY_PACK);
	quest_item.type = CS_QUEST_INVENTORY;
	send(gameinstance->Socket, (char*)&quest_item, quest_item.size, NULL);

	int db_err;
	for (int i = 0; i < MAX_ITEM_COUNT; ++i) {
		SC_ITEM_INFO_PACK item_info{};
		db_err = recv(gameinstance->Socket, (char*)&item_info, item_info.size, NULL);

		if (SOCKET_ERROR == db_err) {
			return false;
		}
		else if (0 == strcmp(item_info._name, "theEnd")) {
			break;
		}
		else {
			// AddItemInfo(FName(item_info._name), (int)(item_info._cnt));
			UE_LOG(LogTemp, Warning, TEXT("[Item name] : %s"), item_info._name);
			continue;
		}
	}
	
	return true;
}
