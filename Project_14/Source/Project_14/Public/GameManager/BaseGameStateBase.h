// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BaseGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API ABaseGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
#pragma region chat
public:
	//chat
	virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS);
#pragma endregion

#pragma region lobby server
public:
	//lobby server (http listener)
	void StartHttpListener(int32 port = 8081);

protected:
	//로비서버용 virtual func
	virtual void OnServerStatusReported(int32 ServerPort, bool bIsIdle);

	private:
	//내부 콜백 함수
	bool HandleServerStatusRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
#pragma endregion

#pragma region game server

public:
	void SendServerStatusToLobby(FString LobbyURL, int32 MyPort, bool bIsIdle);
	void SendGameResultToLobby(bool bIsCleard,float FloatClearTime, FString StringClearTime, const TArray<APlayerState*>& Players);
#pragma endregion
	
};
