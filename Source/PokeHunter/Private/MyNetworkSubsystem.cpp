// Fill out your copyright notice in the Description page of Project Settings.


#include "MyNetworkSubsystem.h"

UMyNetworkSubsystem::UMyNetworkSubsystem()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	// TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
}

void UMyNetworkSubsystem::ConnectToServer()
{
	address = TEXT("127.0.0.1");
	port = 7777;

	FIPv4Address::Parse(address, ip);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPlatformPort(port);
	// addr->SetPort(7777);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Trying to connect.")));

	bool conneted = Socket->Connect(*addr);
}
