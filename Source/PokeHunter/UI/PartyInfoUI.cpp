// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PartyInfoUI.h"

#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UPartyInfoUI::UPartyInfoUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SearchingForGame = false;

	AveragePlayerLatency = 60.0;
}

void UPartyInfoUI::NativeConstruct()
{
	Super::NativeConstruct();

	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
	SendEnterParty();
	// TickSendPartyInfo();
	GetWorld()->GetTimerManager().SetTimer(TH_Partyinfo, this, &UPartyInfoUI::TickSendPartyInfo, 0.1f, true, 0.f);

	// Join Blueprint UI widget 추가 필요
	JoinButton = (UButton*)GetWidgetFromName(TEXT("BTN_READY"));
	FScriptDelegate JoinDelegate;
	JoinDelegate.BindUFunction(this, "OnJoinButtonClicked");
	JoinButton->OnClicked.Add(JoinDelegate);

	// TB_JoinEvent (TextBlock user Widget 추가 필요!)
	JoinEventTextBlock = (UTextBlock*)GetWidgetFromName(TEXT("TB_JoinEvent"));

	GetWorld()->GetTimerManager().SetTimer(SetAveragePlayerLatencyHandle, this, &UPartyInfoUI::SetAveragePlayerLatency, 1.0f, true, 1.0f);

	FString AccessToken;
	UGameInstance* GameInstance = GetGameInstance();
	UBaseInstance* PokeHunterGameInstance = nullptr;
	if (GameInstance != nullptr) {
		PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			AccessToken = PokeHunterGameInstance->AccessToken;

			// Id Access이 아닌 Access Token이 사용된다.
			// AccessToken = PokeHunterGameInstance->IdToken;
		}
	}

	// AccessToken 이 유효한 경우
	if (AccessToken.Len() > 0 && nullptr != PokeHunterGameInstance) {
		TSharedRef<IHttpRequest> GetPlayerDataRequest = gameinstance->HttpModule->CreateRequest();
		GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnGetPlayerDataResponseReceived);
		GetPlayerDataRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/getplayerdata");
		GetPlayerDataRequest->SetVerb("GET");
		
		// GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
		
		GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
		GetPlayerDataRequest->ProcessRequest();
	}
	else {
		/*
		IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
		if (WebBrowserSingleton != nullptr) {
			TOptional<FString> DefaultContext;
			TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
			if (CookieManager.IsValid()) {
				CookieManager->DeleteCookies();
			}
		}
		WebBrowser->LoadURL(LoginUrl);
		FScriptDelegate LoginDelegate;
		LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
		WebBrowser->OnUrlChanged.Add(LoginDelegate);
		*/
	}
}

void UPartyInfoUI::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_Partyinfo);
	GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
	GetWorld()->GetTimerManager().ClearTimer(SetAveragePlayerLatencyHandle);
	Super::NativeDestruct();
}

bool UPartyInfoUI::SendClientState()	// Send this client State
{
	if (false == IsValid(gameinstance))																return false;
	if (0 == gameinstance->PartyListMap.Num())														return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;

	bool retVal = false;
	int32 bSize;

	CS_PARTY_READY_PACK ready_pack;
	ready_pack.size = sizeof(CS_PARTY_READY_PACK);
	ready_pack.type = CS_PARTY_READY;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&ready_pack), ready_pack.size, bSize);
	if (false == retVal) return false;

	SC_PARTY_JOIN_RESULT_PACK result_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&result_pack), sizeof(SC_PARTY_JOIN_RESULT_PACK), bSize);
	if (false == retVal) return false;

	if (-1 != result_pack._result) {
		UE_LOG(LogTemp, Warning, TEXT("Starttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("fail...................."));
		return false;
	}

	return true;
}

bool UPartyInfoUI::SendEnterParty()
{
	if (nullptr == gameinstance)																	return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;
	if (0 == gameinstance->PartyListMap.Num())														return false;
	if (-1 == SelctPartyNumber)																		return false;

	bool retVal = false;
	int32 bSize;

	CS_PARTY_ENTER_PACK enter_pack;
	enter_pack.size = sizeof(CS_PARTY_ENTER_PACK);
	enter_pack.type = CS_PARTY_ENTER;
	enter_pack.party_num = SelctPartyNumber;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&enter_pack), enter_pack.size, bSize);
	if (false == retVal) return false;

	SC_PARTY_ENTER_OK_PACK ok_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_PARTY_ENTER_OK_PACK), bSize);
	if (false == retVal) return false;

	return true;
}

void UPartyInfoUI::TickSendPartyInfo()	// Request Client -> Server
{
	if (nullptr == gameinstance)																	return;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return;
	if (0 == gameinstance->PartyListMap.Num())														return;
	if (-1 == SelctPartyNumber)																		return;

	// 새로운 파티 정보를 위해서 기존의 정보를 전부 지운다.
	if(false == PlayerName.IsEmpty())		PlayerName.Empty();
	if(false == PlayerPetName.IsEmpty())	PlayerPetName.Empty();
	if(false == PartyMemberState.IsEmpty())	PartyMemberState.Empty();

	bool retVal = false;
	int32 bSize;
	
	CS_PARTY_INFO_PACK quest_pack;
	quest_pack.size = sizeof(CS_PARTY_INFO_PACK);
	quest_pack.type = CS_PARTY_INFO;
	quest_pack.party_num = SelctPartyNumber;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_pack), quest_pack.size, bSize);
	if (false == retVal) return;

	SC_PARTY_INFO_PACK info_pack;
	for (int i = 0; i < 4; ++i) {	// 최대 플레이어 수 만큼만 반복한다.
		memset(&info_pack, 0, sizeof(SC_PARTY_INFO_PACK));
		retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&info_pack), sizeof(SC_PARTY_INFO_PACK), bSize);

		// 오류/종료 범위가 큰 순서로 검사
		if (false == retVal)									return;	// 소켓 통신의 실패
		if (0 == strcmp(info_pack._mem, "theEnd"))				break;	// 파티멤버 수신 종료
		if (i == 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;	// 첫 번째 멤버부터 비어있음
		if (i > 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;	// 두 번째 멤버 이후 비어있음

		TCHAR MBTWBuffer[128];
		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._mem, -1, MBTWBuffer, strlen(info_pack._mem));
		
		// 기존 파티 멤버의 준비상태 표시용
		PlayerName.Add(FName(MBTWBuffer));

		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._mem_pet, -1, MBTWBuffer, strlen(info_pack._mem_pet));

		uint8 pet_number = atoi(TCHAR_TO_ANSI(MBTWBuffer));
		PlayerPetName.Add(EPartnerType(pet_number));		// 작동이 안될 경우, TArray의 자료형을 변경
		PartyMemberState.Add(PLAYER_STATE(info_pack._mem_state));
	}

	return;
}

bool UPartyInfoUI::RecvClientJoin()	// [Tick으로 Call!!!] Client에서 AWS GameLift로 접속하라는 신호를 여기로 받음
{
	if (nullptr == gameinstance)	return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState()) return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;
	if (-1 == SelctPartyNumber)		return false;
	bool retVal = false;
	int32 bSize{};

	/*SC_PARTY_JOIN_SUCCESS_PACK start_party_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&start_party_pack), sizeof(SC_PARTY_JOIN_SUCCESS_PACK), bSize);*/

	if (true == retVal) {
		// AWS 실행
		return true;
	}
	else				return false;

	// Clinet에서 명령 실행 대기
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Recv] IOCP server...")));

	return true;
}

bool UPartyInfoUI::LeaveParty()
{
	// GetWorld()->GetTimerManager().PauseTimer(TH_Partyinfo);

	bool retVal = false;
	int32 bSize;

	CS_PARTY_LEAVE_PACK leave_pack;
	leave_pack.size = sizeof(CS_PARTY_LEAVE_PACK);
	leave_pack.type = CS_PARTY_LEAVE;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&leave_pack), leave_pack.size, bSize);

	SC_PARTY_LEAVE_SUCCESS_PACK ok_pack;
	ok_pack.size = sizeof(SC_PARTY_LEAVE_SUCCESS_PACK);
	ok_pack.type = SC_PARTY_LEAVE_SUCCESS;

	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_PARTY_LEAVE_SUCCESS_PACK), bSize);
	if (false == retVal) {
		// GetWorld()->GetTimerManager().UnPauseTimer(TH_Partyinfo);
		return false;
	}

	int32 leave_cnt = PlayerName.Find(FName(ok_pack._mem));

	if (-1 != leave_cnt) {
		PlayerName.RemoveAt(leave_cnt);
		PlayerPetName.RemoveAt(leave_cnt);
		PartyMemberState.RemoveAt(leave_cnt);
		// GetWorld()->GetTimerManager().UnPauseTimer(TH_Partyinfo);
		GetWorld()->GetTimerManager().ClearTimer(TH_Partyinfo);
	}
	else {
		// GetWorld()->GetTimerManager().UnPauseTimer(TH_Partyinfo);
		return false;
	}

	return true;
}

void UPartyInfoUI::SetAveragePlayerLatency() {
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) {
		UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			float TotalPlayerLatency = 0.0f;
			for (float PlayerLatency : PokeHunterGameInstance->PlayerLatencies) {
				TotalPlayerLatency += PlayerLatency;
			}

			if (TotalPlayerLatency > 0) {
				AveragePlayerLatency = TotalPlayerLatency / PokeHunterGameInstance->PlayerLatencies.Num();
			}
		}
	}
}

void UPartyInfoUI::OnJoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);

	if (true == SendClientState()) {
		UE_LOG(LogTemp, Warning, TEXT("[Success] OnJoinButtonClicked()	--->>>	 SendClientState()"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[Fail] OnJoinButtonClicked()		--->>>	 SendClientState()"));
		JoinButton->SetIsEnabled(true);
		return;
	}

	FString AccessToken;
	FString JoinTicketId;
	UGameInstance* GameInstance = GetGameInstance();
	UBaseInstance* PokeHunterGameInstance = nullptr;

	if (GameInstance != nullptr) {
		PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			// AWS Lambda StartMatchmaking Function 테스트 결과 Id Token이 아닌 Access Token이 맞다.
			// ->> 그러나 StartMatchmaking Function 안에서 GetPlayerData Function을 실행할 때, Id Token이 아닌 Access Token을 사용하므로
			// ->> API Gateway에서 Startmachmaking API의 POST 부분에서 Authorization 인증을 제거함

			AccessToken = PokeHunterGameInstance->AccessToken;
			// AccessToken = PokeHunterGameInstance->IdToken;
			JoinTicketId = PokeHunterGameInstance->JoinTicketId;
		}
	}

	if (SearchingForGame) {
		GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
		SearchingForGame = false;

		// if (nullptr != PokeHunterGameInstance && AccessToken.Len() > 0 && JoinTicketId.Len() > 0) {
		if (AccessToken.Len() > 0 && JoinTicketId.Len() > 0) {
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", JoinTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
				TSharedRef<IHttpRequest> StopMatchmakingRequest = gameinstance->HttpModule->CreateRequest();
				StopMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnStopMatchmakingResponseReceived);
				StopMatchmakingRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/stopmatchmaking");
				StopMatchmakingRequest->SetVerb("POST");
				StopMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StopMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StopMatchmakingRequest->SetContentAsString(RequestBody);
				StopMatchmakingRequest->ProcessRequest();
			}
			else {
				UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
				ButtonTextBlock->SetText(FText::FromString("READY"));
				// JoinEventTextBlock->SetText(FText::FromString("READY"));
				JoinButton->SetIsEnabled(true);
			}
		}
		else {
			UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
			ButtonTextBlock->SetText(FText::FromString("READY"));
			// JoinEventTextBlock->SetText(FText::FromString("READY"));
			JoinButton->SetIsEnabled(true);
		}
	}
	else {
		// Player Latency를 연산하는 부분이지만 필요없음?

		if (AccessToken.Len() > 0) {
			TSharedRef<FJsonObject> LatencyMapObj = MakeShareable(new FJsonObject);
			LatencyMapObj->SetNumberField(PokeHunterGameInstance->RegionCode, AveragePlayerLatency);

			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetObjectField("latencyMap", LatencyMapObj);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
				TSharedRef<IHttpRequest> StartJoinRequest = gameinstance->HttpModule->CreateRequest();
				StartJoinRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnStartMatchmakingResponseReceived);
				StartJoinRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/startmatchmaking");
				StartJoinRequest->SetVerb("POST");
				StartJoinRequest->SetHeader("Content-Type", "application/json");
				StartJoinRequest->SetHeader("Authorization", AccessToken);
				StartJoinRequest->SetContentAsString(RequestBody);
				StartJoinRequest->ProcessRequest();
			}
			else {
				JoinButton->SetIsEnabled(true);
			}
		}
		else {
			JoinButton->SetIsEnabled(true);
		}
	}
}

void UPartyInfoUI::PollMatchmaking()
{
	UE_LOG(LogTemp, Warning, TEXT("PollMatchmaking Function"));

	FString AccessToken;
	FString MatchmakingTicketId;
	UGameInstance* GameInstance = GetGameInstance();
	UBaseInstance* PokeHunterGameInstance = nullptr;

	if (GameInstance != nullptr) {
		PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {

			// AccessToken = PokeHunterGameInstance->AccessToken;
			AccessToken = PokeHunterGameInstance->IdToken;

			MatchmakingTicketId = PokeHunterGameInstance->JoinTicketId;
		}
	}

	// if (nullptr != PokeHunterGameInstance && AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0 && SearchingForGame) {
	if (AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0 && SearchingForGame) {
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("ticketId", MatchmakingTicketId);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
			TSharedRef<IHttpRequest> PollMatchmakingRequest = gameinstance->HttpModule->CreateRequest();
			PollMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnPollMatchmakingResponseReceived);
			PollMatchmakingRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/pollmatchmaking");
			PollMatchmakingRequest->SetVerb("POST");
			PollMatchmakingRequest->SetHeader("Content-Type", "application/json");
			PollMatchmakingRequest->SetHeader("Authorization", AccessToken);
			PollMatchmakingRequest->SetContentAsString(RequestBody);
			PollMatchmakingRequest->ProcessRequest();
		}
	}
}

void UPartyInfoUI::OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("playerData")) {

				// UMG에 표시될 데이터들을 설정하지만 필요없으므로 제거.
				// TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("playerData");
				// WebBrowser->SetVisibility(ESlateVisibility::Hidden);

				TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("playerData");
				TSharedPtr<FJsonObject> WinsObject = PlayerData->GetObjectField("Wins");
				TSharedPtr<FJsonObject> LossesObject = PlayerData->GetObjectField("Losses");

				UE_LOG(LogTemp, Warning, TEXT("Get Player Data!"));
			}
		}
	}
}

void UPartyInfoUI::OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("ticketId")) {
				FString MatchmakingTicketId = JsonObject->GetStringField("ticketId");

				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance != nullptr) {
					UBaseInstance* GameLiftTutorialGameInstance = Cast<UBaseInstance>(GameInstance);
					if (GameLiftTutorialGameInstance != nullptr) {
						GameLiftTutorialGameInstance->JoinTicketId = MatchmakingTicketId;

						GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UPartyInfoUI::PollMatchmaking, 1.0f, true);
						SearchingForGame = true;

						UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
						ButtonTextBlock->SetText(FText::FromString("Cancel"));
					}
				}
			}
		}
	}
	JoinButton->SetIsEnabled(true);
}

void UPartyInfoUI::OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) {
		UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			PokeHunterGameInstance->JoinTicketId = "";
		}
	}

	UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
	ButtonTextBlock->SetText(FText::FromString("Stop, READY"));
	// ButtonTextBlock->SetText(FText::FromString("READY"));
	// JoinEventTextBlock->SetText(FText::FromString("Stop READY")); // ->????

	JoinButton->SetIsEnabled(true);
}

void UPartyInfoUI::OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && SearchingForGame) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("ticket")) {
				TSharedPtr<FJsonObject> Ticket = JsonObject->GetObjectField("ticket");
				FString TicketType = Ticket->GetObjectField("Type")->GetStringField("S");

				if (TicketType.Len() > 0) {
					
					// SearchingForGame = false;

					UGameInstance* GameInstance = GetGameInstance();
					if (GameInstance != nullptr) {
						UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
						if (PokeHunterGameInstance != nullptr) {
							PokeHunterGameInstance->JoinTicketId = "";
						}
					}

					UE_LOG(LogTemp, Warning, TEXT("matchmaking event type: %s"), *TicketType);

					if (TicketType.Equals("MatchmakingSucceeded")) {
						
						// MatchmakingSucceeded 일 경우에만 실행
						GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
						SearchingForGame = false;
						
						JoinButton->SetIsEnabled(false);
						JoinEventTextBlock->SetText(FText::FromString("Successfully found a match. Now connecting to the server..."));

						TSharedPtr<FJsonObject> GameSessionInfo = Ticket->GetObjectField("GameSessionInfo")->GetObjectField("M");
						FString IpAddress = GameSessionInfo->GetObjectField("IpAddress")->GetStringField("S");
						FString Port = GameSessionInfo->GetObjectField("Port")->GetStringField("N");

						TArray<TSharedPtr<FJsonValue>> Players = Ticket->GetObjectField("Players")->GetArrayField("L");
						TSharedPtr<FJsonObject> Player = Players[0]->AsObject()->GetObjectField("M");
						FString PlayerSessionId = Player->GetObjectField("PlayerSessionId")->GetStringField("S");
						FString PlayerId = Player->GetObjectField("PlayerId")->GetStringField("S");

						FString LevelName = IpAddress + ":" + Port;
						const FString& Options = "?PlayerSessionId=" + PlayerSessionId + "?PlayerId=" + PlayerId;
						UE_LOG(LogTemp, Warning, TEXT("options: %s"), *Options);

						// SetInputModeGameOnly();

						NativeDestruct();

						UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);
					}
					else {
						// AWS GameLift Dedicated server 접속에 실패하였음.

						// UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
						// ButtonTextBlock->SetText(FText::FromString("FreREADY"));
						
						// JoinEventTextBlock->SetText(FText::FromString(TicketType + ". Please try again"));
					}
				}
			}
		}
	}
}
