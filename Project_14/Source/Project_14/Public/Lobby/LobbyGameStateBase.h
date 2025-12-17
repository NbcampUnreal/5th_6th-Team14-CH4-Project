// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Server/ServerTypes.h"
#include "LobbyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API ALobbyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	TArray<FRoomInfo> RoomList;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	FString GameServerIP;

	void AddRoom(FRoomInfo NewRoom);
	
};
