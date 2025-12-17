// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lobby/LobbyPlayerState.h"
#include "ServerTypes.generated.h"     

/**
 * */
USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	int32 RoomID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	FString RoomName = TEXT("New Room");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	FString HostName = TEXT("");
	
	UPROPERTY(BlueprintReadOnly)
	TArray<ALobbyPlayerState*> MemberPlayerStates;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	int32 CurrentPlayers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	int32 MaxPlayers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	FString GameServerIP = TEXT("127.0.0.1");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	int32 GameServerPort = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	FString MapName = TEXT("MainLevel");
};


class PROJECT_14_API ServerTypes
{
public:
	ServerTypes();
	~ServerTypes();
};