// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/BaseGameStateBase.h"
#include "JsonObjectConverter.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpRouteHandle.h"

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
			return this->HandleGameEndRequest(Request, OnComplete);
		}
	);

	// /api/game_end 경로 바인딩
	HttpRouter->BindRoute(FHttpPath(TEXT("/api/game_end")), EHttpServerRequestVerbs::VERB_POST, RequestHandler);
	HttpServerModule.StartAllListeners();
	
	UE_LOG(LogTemp, Warning, TEXT("[HTTP] Listener Started on Port %d"), Port);
}

void ABaseGameStateBase::OnGameServerFinished(int32 ServerPort)
{
	UE_LOG(LogTemp, Warning, TEXT("[Base] Received GameEnd Signal from Port: %d (No Logic Implemented)"), ServerPort);
}

bool ABaseGameStateBase::HandleGameEndRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FString BodyStr;
	const TArray<uint8>& BodyData = Request.Body;
	BodyStr.Append((const char*)BodyData.GetData(), BodyData.Num());

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyStr);
	TSharedPtr<FJsonObject> JsonObject;

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		int32 FinishedPort = JsonObject->GetIntegerField(TEXT("Port"));

		
		AsyncTask(ENamedThreads::GameThread, [this, FinishedPort]()
		{
			OnGameServerFinished(FinishedPort);
		});

		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("Success"), TEXT("text/plain"));
		OnComplete(MoveTemp(Response));
		return true;
	}
	return false;
}

void ABaseGameStateBase::SendGameEndToLobby(FString LobbyURL, int32 MyPort)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("Port"), MyPort);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(LobbyURL + TEXT("/api/game_end"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
	{
		if(bSuccess) UE_LOG(LogTemp, Log, TEXT("[HTTP] Notification Sent Success"));
	});

	Request->ProcessRequest();
}