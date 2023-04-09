// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyInfoUI.h"

bool UPartyInfoUI::SendClientState()	// Send this client State
{
	if (nullptr == gameinstance)				return false;
	if (0 == gameinstance->PartyListMap.Num())	return false;

	bool retVal = false;
	int32 bSize;

	CS_PARTY_READY_PACK ready_pack;
	ready_pack.size = sizeof(CS_PARTY_READY_PACK);
	ready_pack.type = CS_PARTY_READY;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&ready_pack), ready_pack.size, bSize);
	if (false == retVal) return false;

	return true;
}

bool UPartyInfoUI::TickSendPartyInfo()	// Request Client -> Server
{
	if (nullptr == gameinstance)				return false;
	if (0 == gameinstance->PartyListMap.Num())	return false;
	if (-1 == SelctPartyNumber)					return false;

	// 새로운 파티 정보를 위해서 기존의 정보를 전부 지운다.
	PlayerName.Empty();
	PlayerPetName.Empty();
	PartyMemberState.Empty();

	bool retVal = false;
	int32 bSize;
	
	CS_PARTY_INFO_PACK quest_pack;
	quest_pack.size = sizeof(CS_PARTY_INFO_PACK);
	quest_pack.type = CS_PARTY_INFO;
	quest_pack.party_num = SelctPartyNumber;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_pack), quest_pack.size, bSize);
	if (false == retVal) return false;

	SC_PARTY_INFO_PACK info_pack;
	for (int i = 0; i < 4; ++i) {	// 최대 플레이어 수 만큼만 반복한다.
		memset(&info_pack, 0, sizeof(SC_PARTY_INFO_PACK));
		retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&info_pack), sizeof(SC_PARTY_INFO_PACK), bSize);

		// 오류/종료 범위가 큰 순서로 검사
		if (false == retVal)									return false;	// 소켓 통신의 실패
		if (0 == strcmp(info_pack._mem, "theEnd"))				break;			// 파티멤버 수신 종료
		if (i == 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;			// 첫 번째 멤버부터 비어있음
		if (i > 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;			// 두 번째 멤버 이후 비어있음

		TCHAR MBTWBuffer[128];
		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._mem, -1, MBTWBuffer, strlen(info_pack._mem));
		
		// 기존 파티 멤버의 준비상태 표시용
		PlayerName.Add(FName(MBTWBuffer));
		PlayerPetName.Add(PLAYER_PET(info_pack._mem_pet));		// 작동이 안될 경우, TArray의 자료형을 변경
		PartyMemberState.Add(PLAYER_STATE(info_pack._mem_state));
	}

	return true;
}

bool UPartyInfoUI::RecvClientJoin()	// [Tick으로 Call!!!] Client에서 AWS GameLift로 접속하라는 신호를 여기로 받음
{
	if (nullptr == gameinstance)	return false;
	if (-1 == SelctPartyNumber)		return false;
	bool retVal = false;
	int32 bSize;

	SC_PARTY_JOIN_SUCCESS_PACK start_party_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&start_party_pack), sizeof(SC_PARTY_JOIN_SUCCESS_PACK), bSize);

	if (true == retVal) {
		// AWS 실행
		return true;
	}
	else				return false;

	// Clinet에서 명령 실행 대기
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Recv] IOCP server...")));

	return true;
}
