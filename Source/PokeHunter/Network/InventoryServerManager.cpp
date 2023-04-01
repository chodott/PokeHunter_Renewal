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

	AHunter* MyHunter = Cast<AHunter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	SC_ITEM_INFO_PACK item_info{};

	for (int i = 0; ; ++i) {
		memset(&item_info, 0, sizeof(SC_ITEM_INFO_PACK));
		recv(gameinstance->Socket, (char*)&item_info, sizeof(SC_ITEM_INFO_PACK), NULL);
		FString msg_name = item_info._name;
		int msg_cnt = item_info._cnt;

		if (msg_name == "theEnd")	break;

		// UE_LOG(LogTemp, Warning, TEXT("[Item name] : %s"), *msg_name);
		// UE_LOG(LogTemp, Warning, TEXT("[Item cnt] : %d"), msg_cnt);
		MyHunter->Inventory->InfoArray[i].ItemID = FName(*msg_name);
		MyHunter->Inventory->InfoArray[i].cnt = msg_cnt;
	}
	return true;
}

bool UInventoryServerManager::SaveInventoryDB()
{
	AHunter* MyHunter = Cast<AHunter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	CS_SAVE_INVENTORY_PACK save_pack;
	save_pack.size = sizeof(CS_SAVE_INVENTORY_PACK);
	save_pack.type = CS_SAVE_INVENTORY;

	for (FItemCnter& item : MyHunter->Inventory->InfoArray) {
		strncpy_s(save_pack._name, CHAR_SIZE, TCHAR_TO_ANSI(*item.ItemID.ToString()), strlen(TCHAR_TO_ANSI(*item.ItemID.ToString())));
		save_pack._cnt = item.cnt;
		send(gameinstance->Socket, (char*)&save_pack, save_pack.size, NULL);
	}

	strncpy_s(save_pack._name, CHAR_SIZE, "theEnd", strlen("theEnd"));
	send(gameinstance->Socket, (char*)&save_pack, save_pack.size, NULL);
	return true;
}
