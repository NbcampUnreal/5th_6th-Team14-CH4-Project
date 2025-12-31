// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ServerConfigSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig,Meta = (DisplayName = "Server Network Settings"))
class PROJECT_14_API UServerConfigSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	public:
	//game,lobby server config
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "LobbyServer")
	FString LobbyServerHTTPURL = TEXT("http://127.0.0.1:8081");
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "LobbyServer")
	FString LobbyServerPublicIP = TEXT("127.0.0.1");
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "LobbyServer")
	int32 LobbyServerPort = 7777;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "LobbyServer")
	int32 LobbyHTTPPort = 8081;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameServer")
	FString GameServerPublicIP = TEXT("127.0.0.1");
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameServer")
	TArray<int32> GameServerPorts = {7778,7779};

	//voice chatt config
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Vivox")
	FString VivoxDomain;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Vivox")
	FString VivoxIssuer;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Vivox")
	FString VivoxSecretKey;
	static const UServerConfigSettings* Get()
	{
		return GetDefault<UServerConfigSettings>();
	}
};
