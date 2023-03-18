// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerIntance.h"

UServerIntance::UServerIntance()
{
	reval = WSAStartup(MAKEWORD(2, 2), &wsaData);

	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(PORT_NUM);
	stServerAddr.sin_addr.s_addr = inet_addr("172.28.208.1");
}

bool UServerIntance::ConnectToServer()
{
	if (reval != 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("network fail")));
	else {
		Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (Socket == INVALID_SOCKET) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("socket fail")));
		else {
			reval = connect(Socket, (sockaddr*)&stServerAddr, sizeof(stServerAddr));
			if (reval == SOCKET_ERROR) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("connect fail")));
			else {
				UE_LOG(LogTemp, Warning, TEXT("Success connect server"));
				return true;
			}
		}
	}
	return false;
}
