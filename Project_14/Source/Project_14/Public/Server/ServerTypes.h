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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	int32 HostPlayerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	int32 AssingedServerIndex = -1;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Info")
	bool bIsGameStarting = false;
};

USTRUCT(BlueprintType)
struct FGameServerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server Info")
	FString IPAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server Info")
	int32 Port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server Info")
	bool bIsBusy;

	FGameServerInfo(): IPAddress(TEXT("127.0.0.1")),Port(7777),bIsBusy(false){}
	FGameServerInfo(FString InIP, int32 InPort): IPAddress(InIP),Port(InPort), bIsBusy(false){}

	
};


class PROJECT_14_API ServerTypes
{
public:
	ServerTypes();
	~ServerTypes();
};