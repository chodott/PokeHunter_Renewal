// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeHunter/Network/PartyServerManager.h"

UPartyServerManager::UPartyServerManager()
{
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

bool UPartyServerManager::GetPartyList()
{
	if (nullptr == gameinstance->gSocket) return false;

	int32 bSize = 0;

	CS_PARTY_SEARCHING_PACK searching_pack;
	searching_pack.size = sizeof(CS_PARTY_SEARCHING_PACK);
	searching_pack.type = CS_PARTY_SEARCHING;
	gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&searching_pack), searching_pack.size, bSize);

	// 기존 파티 정보 제거하기.
	gameinstance->PartyListMap.Reset();

	SC_PARTIES_INFO_PACK party_list_pack;
	bool retVal;
	for (int i = 0; ; ++i) {
		memset(&party_list_pack, 0, sizeof(SC_PARTIES_INFO_PACK));
		retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&party_list_pack), sizeof(SC_PARTIES_INFO_PACK), bSize);
		if (false == retVal)		break;
        FName msg_name = party_list_pack._name;
        if (msg_name == "theEnd")	break;

        TCHAR MBTWBuffer[128];
        memset(MBTWBuffer, NULL, 128);
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)party_list_pack._name, -1, MBTWBuffer, strlen(party_list_pack._name));
        msg_name = MBTWBuffer;
        int msg_cnt = party_list_pack._staff_count;

        UE_LOG(LogTemp, Warning, TEXT("[Party name] : %s"), *msg_name.ToString());
		UE_LOG(LogTemp, Warning, TEXT("[member cnt] : %d"), msg_cnt);
		gameinstance->PartyListMap.Add(msg_name, msg_cnt);
	}

	// 현재 World Timer에 GetPartyList() 함수 호출을 등록하여 일정시간마다 PartyListMap을 갱신?
	// GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 60.0f);

	return true;
}
