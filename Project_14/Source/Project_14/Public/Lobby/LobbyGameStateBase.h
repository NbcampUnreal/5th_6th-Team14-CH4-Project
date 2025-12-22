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
	
	virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS) override;
	UPROPERTY(ReplicatedUsing = OnRep_RoomList, BlueprintReadOnly, Category = "Room")
	TArray<FRoomInfo> RoomList;

	/*UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	FString GameServerIP;*/

	UPROPERTY(BlueprintAssignable)
	FOnRoomListUpdated OnRoomListUpdated;

	void AddRoom(FRoomInfo NewRoom);

protected:
	UFUNCTION()
	void OnRep_RoomList();
	
};
