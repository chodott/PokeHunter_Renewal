// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeHunter/Base/BasePokeHunterGameMode.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "UObject/ConstructorHelpers.h"
//#include "GameLiftTutorialHUD.h"
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
		};

		auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSessionObj, void* Params)
		{
			FUpdateGameSessionState* State = (FUpdateGameSessionState*)Params;

			auto Reason = UpdateGameSessionObj.GetUpdateReason();

			if (Reason == Aws::GameLift::Server::Model::UpdateReason::MATCHMAKING_DATA_UPDATED) {

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
					}
				}
				else if (StartGameSessionState.PlayerIdToPlayer.Num() > 0) {
					if (StartGameSessionState.PlayerIdToPlayer.Contains(PlayerId)) {
						auto PlayerObj = StartGameSessionState.PlayerIdToPlayer.Find(PlayerId);
						FString Team = PlayerObj->GetTeam();
						PokeHunterPlayerState->Team = *Team;
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
			// GetWorldTimerManager().SetTimer(PickAWinningTeamHandle, this, &AGameLiftTutorialGameMode::PickAWinningTeam, 1.0f, false, (float)RemainingGameTime);
			// GetWorldTimerManager().SetTimer(SuspendBackfillHandle, this, &ABasePokeHunterGameMode::SuspendBackfill, 1.0f, false, (float)(RemainingGameTime - 60));
			// GetWorldTimerManager().SetTimer(CountDownUntilGameOverHandle, this, &AGameLiftTutorialGameMode::CountDownUntilGameOver, 1.0f, true, 0.0f);
		}
	}
	else if (WaitingForPlayersToJoin) {
		if (TimeSpentWaitingForPlayersToJoin < 5) {		// 게임 시작 설정 5초
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
					
					// int Count = DescribePlayerSessionsResult.GetPlayerSessionsCount();
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

		WaitingForPlayersToJoin = true;
	}
	else if (UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_CANCELLED || UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_COMPLETED 
				|| UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_FAILED || UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_TIMED_OUT)
	{
		LatestBackfillTicketId = "";
		
		TMap<FString, Aws::GameLift::Server::Model::Player> ConnectedPlayers;
		TArray<APlayerState*> PlayerStates = GetWorld()->GetGameState()->PlayerArray;

		for (APlayerState* PlayerState : PlayerStates) {
			if (PlayerState != nullptr) {
				AHunterState* PokeHunterPlayerState = Cast<AHunterState>(PlayerState);
				if (PokeHunterPlayerState != nullptr) {
					auto PlayerObj = ExpectedPlayers.Find(PokeHunterPlayerState->MatchmakingPlayerId);
					if (PlayerObj != nullptr) {
						ConnectedPlayers.Add(PokeHunterPlayerState->MatchmakingPlayerId, *PlayerObj);
					}
				}
			}
		}

		if (ConnectedPlayers.Num() == 0) {
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
