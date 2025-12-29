// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/BaseGameStateBase.h"
#include "JsonObjectConverter.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpRouteHandle.h"
#include "GameFramework/PlayerState.h"

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

	// /api/game_end 경로 바인딩
	HttpRouter->BindRoute(FHttpPath(TEXT("/api/server_status")), EHttpServerRequestVerbs::VERB_POST, RequestHandler);
	HttpServerModule.StartAllListeners();
	
	UE_LOG(LogTemp, Warning, TEXT("[HTTP] Listener Started on Port %d"), Port);
}

void ABaseGameStateBase::OnServerStatusReported(int32 ServerPort, bool bIsIdle)
{
	UE_LOG(LogTemp, Warning, TEXT("[Base] Received GameEnd Signal from Port: %d (No Logic Implemented)"), ServerPort);
}

bool ABaseGameStateBase::HandleServerStatusRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FString BodyStr;
	const TArray<uint8>& BodyData = Request.Body;
	BodyStr.Append((const char*)BodyData.GetData(), BodyData.Num());

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyStr);
	TSharedPtr<FJsonObject> JsonObject;

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		int32 Port = JsonObject->GetIntegerField(TEXT("Port"));

		bool bIsIdle = true;
		if (JsonObject->HasField(TEXT("IsIdle")))
		{
			bIsIdle = JsonObject->GetBoolField(TEXT("IsIdle"));
		}
		AsyncTask(ENamedThreads::GameThread, [this, Port, bIsIdle]()
		{
			OnServerStatusReported(Port,bIsIdle);
		});

		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("Success"), TEXT("text/plain"));
		OnComplete(MoveTemp(Response));
		return true;
	}
	return false;
}

void ABaseGameStateBase::SendServerStatusToLobby(FString LobbyURL, int32 MyPort,bool bIsIdle)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("Port"), MyPort);
	JsonObject->SetBoolField(TEXT("IsIdle"),bIsIdle);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(LobbyURL + TEXT("/api/server_status"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
	{
		if(bSuccess) UE_LOG(LogTemp, Log, TEXT("[HTTP] Notification Sent Success"));
	});

	Request->ProcessRequest();
}

void ABaseGameStateBase::SendGameResultToLobby(bool bIsCleard, float FloatClearTime, FString StringClearTime, const TArray<APlayerState*>& Players)
{
	if (!HasAuthority())
	{
		return;
	}
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	TArray<TSharedPtr<FJsonValue>> PlayerNameArray;
	for (APlayerState* PS : Players)
	{
		if (PS)
		{
			PlayerNameArray.Add(MakeShareable(new FJsonValueString(PS->GetPlayerName())));
		}
		JsonObject->SetArrayField(TEXT("Player_Name"), PlayerNameArray);
		JsonObject->SetStringField(TEXT("str_clear_time"), StringClearTime);
		JsonObject->SetNumberField(TEXT("num_clear_time"), FloatClearTime);
		JsonObject->SetBoolField(TEXT("is_cleared"), bIsCleard);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
		FString LobbyURL = TEXT("http://127.0.0.1:8081");

		Request->SetURL(LobbyURL + TEXT("/api/game_result"));
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetContentAsString(RequestBody);

		Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
		{
			if (bSuccess && Response.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("[Rank] Sent Result Success: %d"), Response->GetResponseCode());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[Rank] Failed to send result."));
			}
		});
		Request->ProcessRequest();
	}
}
