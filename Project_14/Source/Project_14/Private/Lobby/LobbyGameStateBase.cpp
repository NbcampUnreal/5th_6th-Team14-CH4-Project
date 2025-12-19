#include "Lobby/LobbyGameStateBase.h"
#include "Net/UnrealNetwork.h" 


void ALobbyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALobbyGameStateBase, RoomList);
	//DOREPLIFETIME(ALobbyGameStateBase, GameServerIP);
}


TArray<APlayerState*> ALobbyGameStateBase::GetPlayersForChat(APlayerState* SenderPS)
{
	return Super::GetPlayersForChat(SenderPS);
}

void ALobbyGameStateBase::AddRoom(FRoomInfo NewRoom)
{
	
	RoomList.Add(NewRoom);

	if (GetNetMode() != NM_DedicatedServer)
	{
		OnRep_RoomList();
	}
	
	UE_LOG(LogTemp, Log, TEXT("Room Added! ID: %d, Name: %s"), NewRoom.RoomID, *NewRoom.RoomName);
}

void ALobbyGameStateBase::OnRep_RoomList()
{
	if (OnRoomListUpdated.IsBound())
	{
		OnRoomListUpdated.Broadcast();
	}
}
