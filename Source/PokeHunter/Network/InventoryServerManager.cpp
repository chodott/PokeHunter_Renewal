// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryServerManager.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

UInventoryServerManager::UInventoryServerManager()
{
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

bool UInventoryServerManager::GetInvenInfo(ACharacter* myPlayer)
{
	if (nullptr == gameinstance->gSocket) return false;

	int32 bSize{};
	CS_QUEST_INVENTORY_PACK quest_item;
	quest_item.size = sizeof(CS_QUEST_INVENTORY_PACK);
	quest_item.type = CS_QUEST_INVENTORY;
	gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_item), quest_item.size, bSize);

	AHunter* hunter = Cast<AHunter>(myPlayer);
	
	SC_ITEM_INFO_PACK item_info{};
	for (int i = 0; ; ++i) {
		memset(&item_info, 0, sizeof(SC_ITEM_INFO_PACK));
		gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&item_info), sizeof(SC_ITEM_INFO_PACK), bSize);
		FName msg_name = item_info._name;
		int msg_cnt = item_info._cnt;
		if (msg_name == "theEnd") break;

		UE_LOG(LogTemp, Warning, TEXT("[Item name] : %s"), *msg_name.ToString());
		UE_LOG(LogTemp, Warning, TEXT("[Item cnt] : %d"), msg_cnt);

		hunter->Inventory->InfoArray.Add(FItemCnter{ msg_name, msg_cnt });
	}
	return true;
}

bool UInventoryServerManager::SaveInventoryDB()
{ 
	CS_SAVE_INVENTORY_PACK save_pack;
	save_pack.size = sizeof(CS_SAVE_INVENTORY_PACK);
	save_pack.type = CS_SAVE_INVENTORY;

	AHunter* MyHunter = Cast<AHunter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	for (FItemCnter& item : MyHunter->Inventory->InfoArray) {
		strncpy_s(save_pack._name, CHAR_SIZE, TCHAR_TO_ANSI(*item.ItemID.ToString()), strlen(TCHAR_TO_ANSI(*item.ItemID.ToString())));
		save_pack._cnt = item.cnt;
		// send(gameinstance->gSocket, (char*)&save_pack, save_pack.size, NULL);
	}

	strncpy_s(save_pack._name, CHAR_SIZE, "theEnd", strlen("theEnd"));
	// send(gameinstance->gSocket, (char*)&save_pack, save_pack.size, NULL);
	return true;
}
