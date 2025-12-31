// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Server/ServerTypes.h"
#include "BaseGameStateBase.generated.h"

struct FGameResultReport;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaderBoardUpdated);
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
	virtual void StartHttpListener(int32 port = 8081);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_LeaderBoard, BlueprintReadOnly, Category = "Rank")
	TArray<FRankRecord> LeaderBoard;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLeaderBoardUpdated OnLeaderBoardUpdate;

	UFUNCTION(BlueprintCallable, Category = "Rank")
	bool GetRankRecord(int32 Index, FRankRecord& OutRecord)
	{
		if (LeaderBoard.IsValidIndex(Index))
		{
			OutRecord = LeaderBoard[Index];
			return true;
		}
		OutRecord = FRankRecord();
		return false;
	}
	UFUNCTION(BlueprintPure, Category = "Rank")
	static FString FormatClearTime(float TotalSeconds)
	{
		int32 Minutes = FMath::FloorToInt(TotalSeconds/60.f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(TotalSeconds, 60.f));
		return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	}
	
	
protected:
	UFUNCTION()
	void OnRep_LeaderBoard();
	
	//로비서버용 virtual func
	virtual void OnServerStatusReported(int32 ServerPort, bool bIsIdle);
	virtual void OnLeaderBoardUpdated(FGameResultReport RecievedReport);

	private:
	//내부 콜백 함수
	bool HandleServerStatusRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
#pragma endregion

#pragma region game server

public:
	void SendServerStatusToLobby(int32 MyPort, bool bIsIdle);
	void SendGameResultToLobby(bool bIsCleard,float FloatClearTime, FString StringClearTime, const TArray<APlayerState*>& Players);
#pragma endregion
	
};
