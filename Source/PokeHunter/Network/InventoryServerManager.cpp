 // Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "InventoryServerManager.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

UInventoryServerManager::UInventoryServerManager()
{
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

bool UInventoryServerManager::GetInventoryDBInfos()
{
	CS_QUEST_INVENTORY_PACK quest_item;
	quest_item.size = sizeof(CS_QUEST_INVENTORY_PACK);
	quest_item.type = CS_QUEST_INVENTORY;
	send(gameinstance->Socket, (char*)&quest_item, quest_item.size, NULL);

	// ACharacter* MyHunter = UGameplayStatics::GetPlayerCharacter(this, 0);	// 만약 반환값이 nullptr인 경우 GetAllActorsOfClass 사용
	// UGameplayStatics::GetAllActors(GetWorld(), AHunter::StaticClass(), )
	AHunter* MyHunter = Cast<AHunter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	SC_ITEM_INFO_PACK item_info{};
	for (int i = 0; i < 3; ++i) {
		memset(&item_info, 0, sizeof(SC_ITEM_INFO_PACK));
		recv(gameinstance->Socket, (char*)&item_info, sizeof(SC_ITEM_INFO_PACK), NULL);
		FString msg_name = item_info._name;
		int msg_cnt = item_info._cnt;

		// Add Inventory Item
		UE_LOG(LogTemp, Warning, TEXT("[Item name] : %s"), *msg_name);
		UE_LOG(LogTemp, Warning, TEXT("[Item cnt] : %d"), msg_cnt);

		MyHunter->Inventory->InfoArray[i].ItemID = FName(*msg_name);
		MyHunter->Inventory->InfoArray[i].cnt = msg_cnt;
	}

	// [아이템 테스트 확인용 출력]
	// UE_LOG(LogTemp, Warning, TEXT("[Item name] : %s"), *msg_name);
	// UE_LOG(LogTemp, Warning, TEXT("[Item cnt] : %d"), msg_cnt);

	/*
	int db_err;
	SC_ITEM_INFO_PACK item_info{};
	db_err = recv(gameinstance->Socket, (char*)&item_info, sizeof(SC_ITEM_INFO_PACK), NULL);

	for (int i = 0; i < MAX_ITEM_COUNT; ++i) {
		memset(&item_info, 0, sizeof(item_info));
		db_err = recv(gameinstance->Socket, (char*)&item_info, item_info.size, NULL);

		if (SOCKET_ERROR == db_err) {
			return false;
		}
		else if (0 == strcmp(item_info._name, "theEnd")) {
			break;
		}
		else {
			// UItemData itemdata;
			// itemDB->ItemDataClassMap.Add("test");
			UE_LOG(LogTemp, Warning, TEXT("[Item name] : %s"), item_info._name);
			continue;
		}
	}
	*/
	
	return true;
}
