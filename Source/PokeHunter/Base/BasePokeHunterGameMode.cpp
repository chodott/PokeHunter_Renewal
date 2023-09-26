// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeHunter/Base/BasePokeHunterGameMode.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "UObject/ConstructorHelpers.h"
#include "PokeHunterStateBase.h"
#include "HunterState.h"
#include "PokeHunter/MainMenu/TextReaderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"
#include "JsonUtilities.h"

ABasePokeHunterGameMode::ABasePokeHunterGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Hunter/Blueprint/NewHunter"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Hunter/Blueprint/BP_HunterController"));

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		PlayerControllerClass = PlayerControllerBPClass.Class;
		PlayerStateClass = AHunterState::StaticClass();
		GameStateClass = APokeHunterStateBase::StaticClass();
	}

	RemainingGameTime = 240;
	GameSessionActivated = false;

	WaitingForPlayersToJoin = false;
	TimeSpentWaitingForPlayersToJoin = 0;
}

void ABasePokeHunterGameMode::BeginPlay() {
	Super::BeginPlay();

#if WITH_GAMELIFT
	auto InitSDKOutcome = Aws::GameLift::Server::InitSDK();

	if (InitSDKOutcome.IsSuccess()) {
		auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession GameSessionObj, void* Params)
		{
			FStartGameSessionState* State = (FStartGameSessionState*)Params;

			State->Status = Aws::GameLift::Server::ActivateGameSession().IsSuccess();

			FString MatchmakerData = GameSessionObj.GetMatchmakerData();

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MatchmakerData);

			if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
				State->MatchmakingConfigurationArn = JsonObject->GetStringField("matchmakingConfigurationArn");

				TArray<TSharedPtr<FJsonValue>> Teams = JsonObject->GetArrayField("teams");
				for (TSharedPtr<FJsonValue> Team : Teams) {
					TSharedPtr<FJsonObject> TeamObj = Team->AsObject();
					FString TeamName = TeamObj->GetStringField("name");

					TArray<TSharedPtr<FJsonValue>> Players = TeamObj->GetArrayField("players");

					for (TSharedPtr<FJsonValue> Player : Players) {
						TSharedPtr<FJsonObject> PlayerObj = Player->AsObject();
						FString PlayerId = PlayerObj->GetStringField("playerId");

						TSharedPtr<FJsonObject> Attributes = PlayerObj->GetObjectField("attributes");
						TSharedPtr<FJsonObject> Skill = Attributes->GetObjectField("skill");
						FString SkillValue = Skill->GetStringField("valueAttribute");
						auto SkillAttributeValue = new Aws::GameLift::Server::Model::AttributeValue(FCString::Atod(*SkillValue));

						Aws::GameLift::Server::Model::Player AwsPlayerObj;

						AwsPlayerObj.SetPlayerId(TCHAR_TO_ANSI(*PlayerId));
						AwsPlayerObj.SetTeam(TCHAR_TO_ANSI(*TeamName));
						AwsPlayerObj.AddPlayerAttribute("skill", *SkillAttributeValue);

						State->PlayerIdToPlayer.Add(PlayerId, AwsPlayerObj);

						UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
						UE_LOG(LogTemp, Warning, TEXT("[DUG - OnStartGameSession Add PlayerIdToPlayer] PlayerId : %s"), *PlayerId);
						UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
					}
				}
			}
		};

		auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSessionObj, void* Params)
		{
			FUpdateGameSessionState* State = (FUpdateGameSessionState*)Params;

			auto Reason = UpdateGameSessionObj.GetUpdateReason();

			if (Reason == Aws::GameLift::Server::Model::UpdateReason::MATCHMAKING_DATA_UPDATED) {
				State->Reason = EUpdateReason::MATCHMAKING_DATA_UPDATED;

				auto GameSessionObj = UpdateGameSessionObj.GetGameSession();
				FString MatchmakerData = GameSessionObj.GetMatchmakerData();

				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MatchmakerData);

				if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
					TArray<TSharedPtr<FJsonValue>> Teams = JsonObject->GetArrayField("teams");
					for (TSharedPtr<FJsonValue> Team : Teams) {
						TSharedPtr<FJsonObject> TeamObj = Team->AsObject();
						FString TeamName = TeamObj->GetStringField("name");

						TArray<TSharedPtr<FJsonValue>> Players = TeamObj->GetArrayField("players");

						for (TSharedPtr<FJsonValue> Player : Players) {
							TSharedPtr<FJsonObject> PlayerObj = Player->AsObject();
							FString PlayerId = PlayerObj->GetStringField("playerId");

							TSharedPtr<FJsonObject> Attributes = PlayerObj->GetObjectField("attributes");
							TSharedPtr<FJsonObject> Skill = Attributes->GetObjectField("skill");
							FString SkillValue = Skill->GetStringField("valueAttribute");
							auto SkillAttributeValue = new Aws::GameLift::Server::Model::AttributeValue(FCString::Atod(*SkillValue));

							Aws::GameLift::Server::Model::Player AwsPlayerObj;

							AwsPlayerObj.SetPlayerId(TCHAR_TO_ANSI(*PlayerId));
							AwsPlayerObj.SetTeam(TCHAR_TO_ANSI(*TeamName));
							AwsPlayerObj.AddPlayerAttribute("skill", *SkillAttributeValue);

							State->PlayerIdToPlayer.Add(PlayerId, AwsPlayerObj);

							UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
							UE_LOG(LogTemp, Warning, TEXT("[DUG - OnUpdateGameSession Add PlayerIdToPlayer] PlayerId : %s"), *PlayerId);
							UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
						}
					}
				}
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_CANCELLED) {
				State->Reason = EUpdateReason::BACKFILL_CANCELLED;
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_FAILED) {
				State->Reason = EUpdateReason::BACKFILL_FAILED;
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_TIMED_OUT) {
				State->Reason = EUpdateReason::BACKFILL_TIMED_OUT;
			}
		};

		auto OnProcessTerminate = [](void* Params)
		{
			FProcessTerminateState* State = (FProcessTerminateState*)Params;

			auto GetTerminationTimeOutcome = Aws::GameLift::Server::GetTerminationTime();
			if (GetTerminationTimeOutcome.IsSuccess()) {
				State->TerminationTime = GetTerminationTimeOutcome.GetResult();
			}
			State->Status = true;
		};

		auto OnHealthCheck = [](void* Params)
		{
			FHealthCheckState* State = (FHealthCheckState*)Params;
			State->Status = true;

			return State->Status;
		};

		TArray<FString> CommandLineTokens;
		TArray<FString> CommandLineSwitches;
		int Port = FURL::UrlConfig.DefaultPort;

		// GameLiftTutorialServer.exe token -port=7777
		FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

		for (FString Str : CommandLineSwitches) {
			FString Key;
			FString Value;

			if (Str.Split("=", &Key, &Value)) {
				if (Key.Equals("port")) {
					Port = FCString::Atoi(*Value);
				}
			}
		}

		const char* LogFile = "aLogFile.txt";
		const char** LogFiles = &LogFile;
		auto LogParams = new Aws::GameLift::Server::LogParameters(LogFiles, 1);

		auto Params = new Aws::GameLift::Server::ProcessParameters(
			OnStartGameSession,
			&StartGameSessionState,
			OnUpdateGameSession,
			&UpdateGameSessionState,
			OnProcessTerminate,
			&ProcessTerminateState,
			OnHealthCheck,
			&HealthCheckState,
			Port,
			*LogParams
		);

		auto ProcessReadyOutcome = Aws::GameLift::Server::ProcessReady(*Params);
	}
#endif
	GetWorldTimerManager().SetTimer(HandleGameSessionUpdateHandle, this, &ABasePokeHunterGameMode::HandleGameSessionUpdate, 1.0f, true, 5.0f);
	GetWorldTimerManager().SetTimer(HandleProcessTerminationHandle, this, &ABasePokeHunterGameMode::HandleProcessTermination, 1.0f, true, 5.0f);
}

void ABasePokeHunterGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) {
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
#if WITH_GAMELIFT
	if (Options.Len() > 0) {
		const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
		const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");

		if (PlayerSessionId.Len() > 0 && PlayerId.Len() > 0) {
			Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
			DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

			auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
			if (DescribePlayerSessionsOutcome.IsSuccess()) {
				auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
				int Count = 1;
				auto PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
				if (PlayerSessions != nullptr) {
					auto PlayerSession = PlayerSessions[0];
					FString ExpectedPlayerId = PlayerSession.GetPlayerId();
					auto PlayerStatus = PlayerSession.GetStatus();

					if (ExpectedPlayerId.Equals(PlayerId) && PlayerStatus == Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED) {
						auto AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));

						if (!AcceptPlayerSessionOutcome.IsSuccess()) {
							ErrorMessage = "Unauthorized";
						}
					}
					else {
						ErrorMessage = "Unauthorized";
					}
}
				else {
					ErrorMessage = "Unauthorized";
				}
			}
			else {
				ErrorMessage = "Unauthorized";
			}
		}
		else {
			ErrorMessage = "Unauthorized";
		}
	}
	else {
		ErrorMessage = "Unauthorized";
	}
#endif
}

void ABasePokeHunterGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto playerState = Cast<AHunterState>(NewPlayer->PlayerState);
	playerState->InitPlayerData();
}

void ABasePokeHunterGameMode::Logout(AController* Exiting) {
#if WITH_GAMELIFT
	if (LatestBackfillTicketId.Len() > 0) {
		auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
		if (GameSessionIdOutcome.IsSuccess()) {
			FString GameSessionId = GameSessionIdOutcome.GetResult();
			FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
			StopBackfillRequest(GameSessionId, MatchmakingConfigurationArn, LatestBackfillTicketId);
		}
	}
	if (Exiting != nullptr) {
		APlayerState* PlayerState = Exiting->PlayerState;
		if (PlayerState != nullptr) {
			AHunterState* PokeHunterState = Cast<AHunterState>(PlayerState);
			const FString& PlayerSessionId = PokeHunterState->PlayerSessionId;
			if (PlayerSessionId.Len() > 0) {
				Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
			}
		}
	}
#endif
	Super::Logout(Exiting);
}

FString ABasePokeHunterGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString InitializedString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

#if WITH_GAMELIFT
	const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
	const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");

	if (NewPlayerController != nullptr) {
		APlayerState* PlayerState = NewPlayerController->PlayerState;

		if (PlayerState != nullptr) {
			AHunterState* PokeHunterPlayerState = Cast<AHunterState>(PlayerState);
			if (PokeHunterPlayerState != nullptr) {
				PokeHunterPlayerState->PlayerSessionId = *PlayerSessionId;
				PokeHunterPlayerState->MatchmakingPlayerId = *PlayerId;

				if (UpdateGameSessionState.PlayerIdToPlayer.Num() > 0) {
					if (UpdateGameSessionState.PlayerIdToPlayer.Contains(PlayerId)) {
						auto PlayerObj = UpdateGameSessionState.PlayerIdToPlayer.Find(PlayerId);
						FString Team = PlayerObj->GetTeam();
						PokeHunterPlayerState->Team = *Team;

						UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
						UE_LOG(LogTemp, Warning, TEXT("[DUG - UpdateGameSessionState GetTeam PlayerIdToPlayer] Team : %s"), *Team);
						UE_LOG(LogTemp, Warning, TEXT("[DUG - UpdateGameSessionState GetTeam PlayerIdToPlayer] PlayerId : %s"), *PlayerId);
						UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
					}
				}
				else if (StartGameSessionState.PlayerIdToPlayer.Num() > 0) {
					if (StartGameSessionState.PlayerIdToPlayer.Contains(PlayerId)) {
						auto PlayerObj = StartGameSessionState.PlayerIdToPlayer.Find(PlayerId);
						FString Team = PlayerObj->GetTeam();
						PokeHunterPlayerState->Team = *Team;

						UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
						UE_LOG(LogTemp, Warning, TEXT("[DUG - StartGameSessionState GetTeam PlayerIdToPlayer] Team : %s"), *Team);
						UE_LOG(LogTemp, Warning, TEXT("[DUG - UpdateGameSessionState GetTeam PlayerIdToPlayer] PlayerId : %s"), *PlayerId);
						UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
					}
				}
			}
		}
	}
#endif
	return InitializedString;
}

void ABasePokeHunterGameMode::EndGame()
{
	GetWorldTimerManager().ClearTimer(EndGameHandle);
	GetWorldTimerManager().ClearTimer(HandleProcessTerminationHandle);
	GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);

#if WITH_GAMELIFT
	Aws::GameLift::Server::TerminateGameSession();
	Aws::GameLift::Server::ProcessEnding();
	FGenericPlatformMisc::RequestExit(false);
#endif
}

void ABasePokeHunterGameMode::HandleProcessTermination()
{
	if (ProcessTerminateState.Status) {
		GetWorldTimerManager().ClearTimer(HandleProcessTerminationHandle);
		GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);
		GetWorldTimerManager().ClearTimer(SuspendBackfillHandle);

		FString ProcessInterruptionMessage;

		if (ProcessTerminateState.TerminationTime <= 0L) {
			ProcessInterruptionMessage = "Server process could shut down at any time";
		}
		else {
			long TimeLeft = (long)(ProcessTerminateState.TerminationTime - FDateTime::Now().ToUnixTimestamp());
			ProcessInterruptionMessage = FString::Printf(TEXT("Server process scheduled to terminate in %ld seconds"), TimeLeft);
		}

		/*if (GameState != nullptr) {
			ABasePokeHunterGameMode* PokeHunterGameState = Cast<ABasePokeHunterGameMode>(GameState);
			if (PokeHunterGameState != nullptr) {
				PokeHunterGameState->LatestEvent = ProcessInterruptionMessage;
			}
		}*/

		GetWorldTimerManager().SetTimer(EndGameHandle, this, &ABasePokeHunterGameMode::EndGame, 1.0f, false, 10.0f);
	}
}

void ABasePokeHunterGameMode::HandleGameSessionUpdate()
{
#if WITH_GAMELIFT
	if (!GameSessionActivated) {
		if (StartGameSessionState.Status) {
			GameSessionActivated = true;
			ExpectedPlayers = StartGameSessionState.PlayerIdToPlayer;
			WaitingForPlayersToJoin = true;

			int32 ticket_cnt = ExpectedPlayers.Num();
			UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
			UE_LOG(LogTemp, Warning, TEXT("[DUG - First GameSessionActivated] ExpectedPlayers cnt: %d"), ticket_cnt);
			UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));

			// GetWorldTimerManager().SetTimer(PickAWinningTeamHandle, this, &AGameLiftTutorialGameMode::PickAWinningTeam, 1.0f, false, (float)RemainingGameTime);
			// GetWorldTimerManager().SetTimer(SuspendBackfillHandle, this, &ABasePokeHunterGameMode::SuspendBackfill, 1.0f, false, (float)(RemainingGameTime - 60));
			// GetWorldTimerManager().SetTimer(CountDownUntilGameOverHandle, this, &AGameLiftTutorialGameMode::CountDownUntilGameOver, 1.0f, true, 0.0f);
		}
	}
	else if (WaitingForPlayersToJoin) {
		if (TimeSpentWaitingForPlayersToJoin < 10) {		// 게임 시작 설정 10초
			auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();	// 현재 Game Session ID를 가져옴
			if (GameSessionIdOutcome.IsSuccess()) {									// 가져오기 성공
				FString GameSessionId = FString(GameSessionIdOutcome.GetResult());

				Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
				DescribePlayerSessionsRequest.SetGameSessionId(TCHAR_TO_ANSI(*GameSessionId));
				DescribePlayerSessionsRequest.SetPlayerSessionStatusFilter("RESERVED");

				auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
				if (DescribePlayerSessionsOutcome.IsSuccess()) {
					auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
					
					Aws::GameLift::Server::Model::DescribePlayerSessionsResult;
					
					int Count;
					auto PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
					if (Count == 0) {
						UpdateGameSessionState.Reason = EUpdateReason::BACKFILL_COMPLETED;

						WaitingForPlayersToJoin = false;
						TimeSpentWaitingForPlayersToJoin = 0;
					}
					else {
						TimeSpentWaitingForPlayersToJoin++;
					}
				}
				else {
					TimeSpentWaitingForPlayersToJoin++;
				}
			}
			else {
				TimeSpentWaitingForPlayersToJoin++;
			}
		}
		else {
			UpdateGameSessionState.Reason = EUpdateReason::BACKFILL_COMPLETED;

			WaitingForPlayersToJoin = false;
			TimeSpentWaitingForPlayersToJoin = 0;
		}
	}
	else if (UpdateGameSessionState.Reason == EUpdateReason::MATCHMAKING_DATA_UPDATED) {
		LatestBackfillTicketId = "";
		ExpectedPlayers = UpdateGameSessionState.PlayerIdToPlayer;

		int32 ticket_cnt = ExpectedPlayers.Num();
		UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
		UE_LOG(LogTemp, Warning, TEXT("[DUG - 0] UpdateGameSessionState.PlayerIdToPlayer -->> ExpectedPlayers cnt: %d"), ticket_cnt);
		UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));

		WaitingForPlayersToJoin = true;
	}
	else if (UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_CANCELLED || UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_COMPLETED 
				|| UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_FAILED || UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_TIMED_OUT)
	{
		LatestBackfillTicketId = "";

		int32 ticket_cnt = ExpectedPlayers.Num();
		UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));
		UE_LOG(LogTemp, Warning, TEXT("[DUG - 0] ExpectedPlayers cnt: %d"), ticket_cnt);
		
		TMap<FString, Aws::GameLift::Server::Model::Player> ConnectedPlayers;
		TArray<APlayerState*> PlayerStates = GetWorld()->GetGameState()->PlayerArray;

		int32 playerStateCnt = PlayerStates.Num();
		UE_LOG(LogTemp, Warning, TEXT("[DUG - 0] PlayerState(WorldPlayerArray) cnt: %d"), playerStateCnt);

		for (APlayerState* PlayerState : PlayerStates) {
			if (PlayerState != nullptr) {
				AHunterState* PokeHunterPlayerState = Cast<AHunterState>(PlayerState);
				UE_LOG(LogTemp, Warning, TEXT("[DUG - 1] MatchmakingPlayerId : %s"), *PokeHunterPlayerState->MatchmakingPlayerId);

				if (PokeHunterPlayerState != nullptr) {
					auto PlayerObj = ExpectedPlayers.Find(PokeHunterPlayerState->MatchmakingPlayerId);
					UE_LOG(LogTemp, Warning, TEXT("[DUG - 2] [PlayerObj] MatchmakingPlayerId : %s"), *PokeHunterPlayerState->MatchmakingPlayerId);
					if (PlayerObj != nullptr) {
						UE_LOG(LogTemp, Warning, TEXT("[DUG - 3] [ConnectedPlayers] Add Actor"));
						ConnectedPlayers.Add(PokeHunterPlayerState->MatchmakingPlayerId, *PlayerObj);
					}
				}
			}
		}
		int32 connectedPlayer_cnt = ConnectedPlayers.Num();
		UE_LOG(LogTemp, Warning, TEXT("[DUG - 4] ConnectedPlayers cnt: %d"), connectedPlayer_cnt);
		UE_LOG(LogTemp, Warning, TEXT("========================================================================================"));

		if (ConnectedPlayers.Num() == 0) {
			UE_LOG(LogTemp, Warning, TEXT("[DUG - Ending] - Call EndGame() Function!"));
			EndGame();
		}
		/*else if (ConnectedPlayers.Num() < 4) {
			auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
			if (GameSessionIdOutcome.IsSuccess()) {
				FString GameSessionId = FString(GameSessionIdOutcome.GetResult());
				FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
				LatestBackfillTicketId = CreateBackfillRequest(GameSessionId, MatchmakingConfigurationArn, ConnectedPlayers);
				if (LatestBackfillTicketId.Len() > 0) {
					UpdateGameSessionState.Reason = EUpdateReason::BACKFILL_INITIATED;
				}
			}
		}*/
	}
#endif
}

void ABasePokeHunterGameMode::SuspendBackfill() {
	GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);
#if WITH_GAMELIFT
	if (LatestBackfillTicketId.Len() > 0) {
		auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
		if (GameSessionIdOutcome.IsSuccess()) {
			FString GameSessionId = GameSessionIdOutcome.GetResult();
			FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
			if (!StopBackfillRequest(GameSessionId, MatchmakingConfigurationArn, LatestBackfillTicketId)) {
				GetWorldTimerManager().SetTimer(SuspendBackfillHandle, this, &ABasePokeHunterGameMode::SuspendBackfill, 1.0f, false, 1.0f);
			}
		}
		else {
			GetWorldTimerManager().SetTimer(SuspendBackfillHandle, this, &ABasePokeHunterGameMode::SuspendBackfill, 1.0f, false, 1.0f);
		}
	}
#endif
}

FString ABasePokeHunterGameMode::CreateBackfillRequest(FString GameSessionArn, FString MatchmakingConfigurationArn, TMap<FString, Aws::GameLift::Server::Model::Player> Players)
{
#if WITH_GAMELIFT
	Aws::GameLift::Server::Model::StartMatchBackfillRequest StartMatchBackfillRequest;
	StartMatchBackfillRequest.SetGameSessionArn(TCHAR_TO_ANSI(*GameSessionArn));
	StartMatchBackfillRequest.SetMatchmakingConfigurationArn(TCHAR_TO_ANSI(*MatchmakingConfigurationArn));

	for (auto& Elem : Players) {
		auto PlayerObj = Elem.Value;
		StartMatchBackfillRequest.AddPlayer(PlayerObj);
	}

	auto StartMatchBackfillOutcome = Aws::GameLift::Server::StartMatchBackfill(StartMatchBackfillRequest);
	if (StartMatchBackfillOutcome.IsSuccess()) {
		return StartMatchBackfillOutcome.GetResult().GetTicketId();
	}
	else {
		return "";
	}
#endif
	return "";
}

bool ABasePokeHunterGameMode::StopBackfillRequest(FString GameSessionArn, FString MatchmakingConfigurationArn, FString TicketId)
{
#if WITH_GAMELIFT
	Aws::GameLift::Server::Model::StopMatchBackfillRequest StopMatchBackfillRequest;
	StopMatchBackfillRequest.SetGameSessionArn(TCHAR_TO_ANSI(*GameSessionArn));
	StopMatchBackfillRequest.SetMatchmakingConfigurationArn(TCHAR_TO_ANSI(*MatchmakingConfigurationArn));
	StopMatchBackfillRequest.SetTicketId(TCHAR_TO_ANSI(*TicketId));

	auto StopMatchBackfillOutcome = Aws::GameLift::Server::StopMatchBackfill(StopMatchBackfillRequest);

	return StopMatchBackfillOutcome.IsSuccess();
#endif
	return false;
}
