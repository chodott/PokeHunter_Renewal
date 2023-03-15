// Fill out your copyright notice in the Description page of Project Settings.
#pragma once


#include "InventoryServerManager.h"

UInventoryServerManager::UInventoryServerManager()
{
	for (FItemCnter& item : InfoArray) {
		if (0 < item.cnt) {

		}
	}
}

bool UInventoryServerManager::GetInventoryDBInfos()
{
	CS_QUEST_INVENTORY_PACK quest_item;
	quest_item.size = sizeof(CS_QUEST_INVENTORY_PACK);
	quest_item.type = CS_QUEST_INVENTORY;
	send(Socket, (char*)&quest_item, quest_item.size, NULL);

	int db_err;
	for (int i = 0; ; ++i) {
		SC_ITEM_INFO_PACK item_info;
		db_err = recv(Socket, (char*)&item_info, item_info.size, NULL);

		if (SOCKET_ERROR == db_err) {
			return false;
		}
		else if (0 == strcmp(item_info._name, "theEnd")) {
			break;
		}
		else {
			
			continue;
		}
	}
	
	return true;
}
