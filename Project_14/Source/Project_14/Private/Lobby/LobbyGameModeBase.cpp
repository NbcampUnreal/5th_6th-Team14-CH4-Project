// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameModeBase.h"
#include "Lobby/LobbyGameStateBase.h"
#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyPlayerState.h"


ALobbyGameModeBase::ALobbyGameModeBase()
{
	PlayerStateClass = ALobbyPlayerState::StaticClass();
}

void ALobbyGameModeBase::StartGameForRoom(const FRoomInfo& RoomData)
{
	for (ALobbyPlayerState* PS: RoomData.MemberPlayerStates)
	{
		if (PS)
		{
			ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(PS->GetOwner());
			if (IsValid(PC) == true)
			{
				FString GameServerIpPort = GameServerIP + ":" + FString::FromInt(RoomData.GameServerPort);
				PC->ClientRPC_MoveToGameServer(GameServerIpPort);
			}
		}
	}
}

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ALobbyGameStateBase* GS = GetGameState<ALobbyGameStateBase>())
	{
		GS->AddLobbyUser(NewPlayer->PlayerState);
	}
}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	

	if (ALobbyGameStateBase* GS = GetGameState<ALobbyGameStateBase>())
	{
		GS->ProcessPlayerLogout(Exiting->PlayerState);
	}
	Super::Logout(Exiting);
}
