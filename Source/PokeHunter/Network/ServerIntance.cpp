// Fill out your copyright notice in the Description page of Project Settings.
/*
#include "ServerIntance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "PokeHunter\MainMenu\TextReaderComponent.h"

UServerIntance::UServerIntance()
{
	reval = WSAStartup(MAKEWORD(2, 2), &wsaData);

	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(PORT_NUM);
	// stServerAddr.sin_addr.s_addr = inet_addr("172.28.208.1");

	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");

	HttpModule = &FHttpModule::Get();
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

void UServerIntance::Shutdown()
{
	Super::Shutdown();

	if (AccessToken.Len() > 0) {
		TSharedRef<IHttpRequest> InvalidateTokensRequest = HttpModule->CreateRequest();
		InvalidateTokensRequest->SetURL(ApiUrl + "/invalidatetokens");
		InvalidateTokensRequest->SetVerb("GET");
		InvalidateTokensRequest->SetHeader("Content-Type", "application/json");
		InvalidateTokensRequest->SetHeader("Authorization", AccessToken);
		InvalidateTokensRequest->ProcessRequest();
	}
}

void UServerIntance::SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken)
{
	AccessToken = NewAccessToken;
	IdToken = NewIdToken;
	RefreshToken = NewRefreshToken;

	UE_LOG(LogTemp, Warning, TEXT("access token: %s"), *AccessToken);
	UE_LOG(LogTemp, Warning, TEXT("refresh token: %s"), *RefreshToken);

	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UServerIntance::RetrieveNewTokens, 1.0f, false, 60.0f);
}

void UServerIntance::RetrieveNewTokens()
{
	if (AccessToken.Len() > 0 && RefreshToken.Len() > 0) {
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("refreshToken", RefreshToken);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
			TSharedRef<IHttpRequest> RetrieveNewTokensRequest = HttpModule->CreateRequest();
			RetrieveNewTokensRequest->OnProcessRequestComplete().BindUObject(this, &UServerIntance::OnRetrieveNewTokensResponseReceived);
			RetrieveNewTokensRequest->SetURL(ApiUrl + "/retrievenewtokens");
			RetrieveNewTokensRequest->SetVerb("POST");
			RetrieveNewTokensRequest->SetHeader("Content-Type", "application/json");
			RetrieveNewTokensRequest->SetHeader("Authorization", AccessToken);
			RetrieveNewTokensRequest->SetContentAsString(RequestBody);
			RetrieveNewTokensRequest->ProcessRequest();
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UServerIntance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
}

void UServerIntance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (!JsonObject->HasField("error")) {
				SetCognitoTokens(JsonObject->GetStringField("accessToken"), JsonObject->GetStringField("idToken"), RefreshToken);
			}
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UServerIntance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UServerIntance::RetrieveNewTokens, 1.0f, false, 30.0f);
	}
}
*/