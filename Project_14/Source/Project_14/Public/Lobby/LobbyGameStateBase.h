// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameManager/BaseGameStateBase.h"
#include "Server/ServerTypes.h"
#include "LobbyGameStateBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoomListUpdated);
/**
 * 
 */
UCLASS()
class PROJECT_14_API ALobbyGameStateBase : public ABaseGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<APlayerState*> LobbyUsers;
	
	UPROPERTY(ReplicatedUsing = OnRep_RoomList, BlueprintReadOnly, Category = "Lobby Data")
	TArray<FRoomInfo> RoomList;

	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Lobby Data")
	TArray<FGameServerInfo> GameServerList;
	
	UPROPERTY(BlueprintAssignable)
	FOnRoomListUpdated OnRoomListUpdated;

	virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS) override;

	//삭제 예정=====================
	void AddRoom(FRoomInfo NewRoom);
//===========================================
	void AddLobbyUser(APlayerState* NewUser);
	void ProcessPlayerLogout(APlayerState* ExitingPS);

	int32 RoomIDCounter = 0;
	int32 GetRoomIndexByID(int32 InRoomID);
	int32 CreateRoom(ALobbyPlayerState* Host,FString RoomName);
	bool JoinRoom(int32 RoomID, ALobbyPlayerState* Joiner);
	bool LeaveRoom(ALobbyPlayerState* Leaver);
	void SetRoomGameStarting(int32 RoomID);

	int32 FindAvailableServerIndex();
	void SetServerBusyStatus(int32 ServerID, bool bBusy);
	void InitializeServerList();

protected:

	virtual void BeginPlay() override;
	UFUNCTION()
	void OnRep_RoomList();
	
};
