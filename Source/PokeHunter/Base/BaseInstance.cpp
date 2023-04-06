// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "PokeHunter/MainMenu/TextReaderComponent.h"

UBaseInstance::UBaseInstance()
{
	// gSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
	// addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));
	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");
	HttpModule = &FHttpModule::Get();
}

bool UBaseInstance::ConnectToServer(FString server_addr)
{
	FIPv4Address::Parse(server_addr, ip);
	addr->SetIp(ip.Value);
	addr->SetPort(PORT_NUM);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Trying to connect.")));

	if (true == gSocket->Connect(*addr)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Success] connect to server")));
		return true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Fail] connect to server")));
		return false;
	}
}

// 클라이언트 종료시에 호출되는 함수
void UBaseInstance::Shutdown()
{
	Super::Shutdown();

	if (AccessToken.Len() > 0) {
		TSharedRef<IHttpRequest> InvalidateTokensRequest = HttpModule->CreateRequest();
		InvalidateTokensRequest->SetURL(ApiUrl + "/invalidatetokens");
		InvalidateTokensRequest->SetVerb("GET");
		InvalidateTokensRequest->SetHeader("Content-Type", "application/json");
		InvalidateTokensRequest->SetHeader("Authorization", IdToken);
		InvalidateTokensRequest->ProcessRequest();
	}
}

void UBaseInstance::SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken)
{
	NewAccessToken.ReplaceInline(TEXT("\n"), TEXT(""));
	AccessToken = NewAccessToken;

	NewIdToken.ReplaceInline(TEXT("\n"), TEXT(""));
	IdToken = NewIdToken;
	
	NewRefreshToken.ReplaceInline(TEXT("\n"), TEXT(""));
	RefreshToken = NewRefreshToken;

	UE_LOG(LogTemp, Warning, TEXT("[X]access token: %s"), *AccessToken);
	UE_LOG(LogTemp, Warning, TEXT("[O]IdToken token: %s"), *IdToken);
	UE_LOG(LogTemp, Warning, TEXT("refresh token: %s"), *RefreshToken);

	// World Timer에 등록하기
	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 60.0f);
}

void UBaseInstance::RetrieveNewTokens()
{
	if (IdToken.Len() > 0 && RefreshToken.Len() > 0) {
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("refreshToken", RefreshToken);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
			TSharedRef<IHttpRequest> RetrieveNewTokensRequest = HttpModule->CreateRequest();
			RetrieveNewTokensRequest->OnProcessRequestComplete().BindUObject(this, &UBaseInstance::OnRetrieveNewTokensResponseReceived);
			RetrieveNewTokensRequest->SetURL(ApiUrl + "/retrievenewtokens");
			RetrieveNewTokensRequest->SetVerb("POST");
			RetrieveNewTokensRequest->SetHeader("Content-Type", "application/json");
			
			// RetrieveNewTokensRequest->SetHeader("Authorization", AccessToken);
			RetrieveNewTokensRequest->SetHeader("Authorization", IdToken);
			
			RetrieveNewTokensRequest->SetContentAsString(RequestBody);
			RetrieveNewTokensRequest->ProcessRequest();
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
}

void UBaseInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (!JsonObject->HasField("error") || !JsonObject->HasField("Unauthorized")) {
				SetCognitoTokens(JsonObject->GetStringField("accessToken"), JsonObject->GetStringField("idToken"), RefreshToken);
			}
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
	}
}
