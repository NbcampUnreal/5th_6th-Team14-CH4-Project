// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/BaseGameStateBase.h"
#include "JsonObjectConverter.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpRouteHandle.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Server/ServerConfigSettings.h"
#include "Server/ServerTypes.h"

TArray<APlayerState*> ABaseGameStateBase::GetPlayersForChat(APlayerState* SenderPS)
{
	return PlayerArray;
}

void ABaseGameStateBase::StartHttpListener(int32 Port)
{
	if (!HasAuthority())
	{
		return;
	}

	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(Port);

	if (!HttpRouter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[HTTP] Failed to bind Router on port %d"), Port);
		return;
	}

	FHttpRequestHandler RequestHandler = FHttpRequestHandler::CreateLambda(
		[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) -> bool
		{
			return this->HandleServerStatusRequest(Request, OnComplete);
		}
	);


	HttpRouter->BindRoute(FHttpPath(TEXT("/api/server_status")), EHttpServerRequestVerbs::VERB_POST, RequestHandler);
	HttpServerModule.StartAllListeners();

	UE_LOG(LogTemp, Warning, TEXT("[HTTP] Listener Started on Port %d"), Port);
}

void ABaseGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseGameStateBase, LeaderBoard);
}


void ABaseGameStateBase::OnRep_LeaderBoard()
{
	if (OnLeaderBoardUpdate.IsBound())
	{
		OnLeaderBoardUpdate.Broadcast();
	}
}

void ABaseGameStateBase::OnServerStatusReported(int32 ServerPort, bool bIsIdle)
{
	UE_LOG(LogTemp, Warning, TEXT("[Base] Received GameEnd Signal from Port: %d (No Logic Implemented)"), ServerPort);
}

void ABaseGameStateBase::OnLeaderBoardUpdated(FGameResultReport RecievedReport)
{
	UE_LOG(LogTemp, Warning, TEXT("[Base] Received LeaderBoardUpdate signal"));

}

bool ABaseGameStateBase::HandleServerStatusRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FString BodyStr;
	const TArray<uint8>& BodyData = Request.Body;
	BodyStr.Append((const char*)BodyData.GetData(), BodyData.Num());


	FServerStatusReport ReceivedStatus;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(BodyStr, &ReceivedStatus, 0, 0))
	{
		AsyncTask(ENamedThreads::GameThread, [this, ReceivedStatus]()
			{
				OnServerStatusReported(ReceivedStatus.Port, ReceivedStatus.IsIdle);
			});

		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("Success"), TEXT("text/plain"));
		OnComplete(MoveTemp(Response));
		return true;
	}

	return false;
}

void ABaseGameStateBase::SendServerStatusToLobby(int32 MyPort, bool bIsIdle)
{
	if (!HasAuthority()) return;

	FServerStatusReport StatusData;
	StatusData.Port = MyPort;
	StatusData.IsIdle = bIsIdle;

	FString RequestBody;
	if (!FJsonObjectConverter::UStructToJsonObjectString(StatusData, RequestBody))
	{
		UE_LOG(LogTemp, Error, TEXT("[HTTP] Failed to serialize StatusData"));
		return;
	}

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	const UServerConfigSettings* Settings = UServerConfigSettings::Get();
	FString BaseAddress = Settings->LobbyServerHTTPURL;
	FString FullURL = FString::Printf(TEXT("http://%s/api/server_status"), *BaseAddress);
	Request->SetURL(FullURL);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
		{
			if (bSuccess) UE_LOG(LogTemp, Log, TEXT("[HTTP] Status Notification Sent"));
		});

	Request->ProcessRequest();
}

void ABaseGameStateBase::SendGameResultToLobby(bool bIsCleared, float FloatClearTime, FString StringClearTime, const TArray<APlayerState*>& Players)
{
	if (!HasAuthority()) return;


	FGameResultReport ResultReport;
	ResultReport.is_cleared = bIsCleared;
	ResultReport.num_clear_time = FloatClearTime;

	for (APlayerState* PS : Players)
	{
		if (PS) ResultReport.player_names.Add(PS->GetPlayerName());
	}

	FString RequestBody;
	if (!FJsonObjectConverter::UStructToJsonObjectString(ResultReport, RequestBody))
	{
		return;
	}
	const UServerConfigSettings* Settings = UServerConfigSettings::Get();
	FString BaseAddress = Settings->LobbyServerHTTPURL;
	FString FullURL = FString::Printf(TEXT("http://%s/api/game_result"), *BaseAddress);

	UE_LOG(LogTemp, Log, TEXT("[HTTP] Sending Status to: %s"), *FullURL);

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb(TEXT("POST"));
	Request->SetURL(FullURL);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);

	Request->ProcessRequest();
}
