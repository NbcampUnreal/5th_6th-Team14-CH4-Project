// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lobby/LobbyPlayerState.h"
#include "ServerTypes.generated.h"     


/*
 * 게임 종료 사유 정의
 */
UENUM(BlueprintType)
enum class EGameEndReason : uint8
{
	None,
	Cleared,        // 클리어 
	Forfeited,      // 누군가 포기/탈주 
	GivenUp         // 내가 포기 버튼 누름 
};

USTRUCT(BlueprintType)
struct FRankRecord
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category="Rank")
	TArray<FString> PlayerNames;
	UPROPERTY(BlueprintReadOnly, Category="Rank")
	float ClearTime;

	FRankRecord():ClearTime(0.0f){}
	FRankRecord(const TArray<FString>& InNames, const float InTime):PlayerNames(InNames),ClearTime(InTime){}
};

/**
 *방 정보 구조체
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

/*
 * 게임 서버 정보 구조체
 */
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