// Fill out your copyright notice in the Description page of Project Settings.


#include "MyNetworkSubsystem.h"

UMyNetworkSubsystem::UMyNetworkSubsystem()
{
	reval = WSAStartup(MAKEWORD(2, 2), &wsaData);

	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(PORT_NUM);
	stServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

FU_SC_LOGIN_INFO_PACK UMyNetworkSubsystem::ConnectToServer(FString in_id, FString in_pw)
{
	FU_SC_LOGIN_INFO_PACK u_info_pack;
	SC_LOGIN_INFO_PACK info_pack;

	if (reval != 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("network fail")));
	else {
		Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (Socket == INVALID_SOCKET) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("socket fail")));
		else {
			reval = connect(Socket, (sockaddr*)&stServerAddr, sizeof(stServerAddr));
			if (reval == SOCKET_ERROR) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("connect fail")));
			else {
				CS_LOGIN_PACK msg;
				msg.size = sizeof(CS_LOGIN_PACK);
				msg.type = CS_LOGIN;
				strcpy(msg.id, TCHAR_TO_ANSI(*in_id));
				strcpy(msg.pw, TCHAR_TO_ANSI(*in_pw));
				send(Socket, (char*)&msg, msg.size, NULL);

				SC_LOGIN_SUCCESS_PACK ok_pack;
				recv(Socket, (char*)&ok_pack, sizeof(SC_LOGIN_SUCCESS_PACK), NULL);
				if (SC_LOGIN_SUCCESS == ok_pack.type) {
					recv(Socket, (char*)&info_pack, sizeof(SC_LOGIN_INFO_PACK), NULL);
					
					u_info_pack.name = ANSI_TO_TCHAR(info_pack.name);
					/*u_info_pack._player_skin[0] = info_pack._player_skin;
					u_info_pack._pet_num[0] = info_pack._pet_num;
					u_info_pack.q_item[0] = info_pack.q_item;*/
					u_info_pack.q_skill = ANSI_TO_TCHAR(info_pack.q_skill);

					return u_info_pack;
				}
			}
		}
	}

	u_info_pack.name = ANSI_TO_TCHAR("empty");
	return u_info_pack;
}
