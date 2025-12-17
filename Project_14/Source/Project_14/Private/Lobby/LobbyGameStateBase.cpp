#include "Lobby/LobbyGameStateBase.h"
#include "Net/UnrealNetwork.h" 


void ALobbyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALobbyGameStateBase, RoomList);
	DOREPLIFETIME(ALobbyGameStateBase, GameServerIP);
}


void ALobbyGameStateBase::AddRoom(FRoomInfo NewRoom)
{
	
	RoomList.Add(NewRoom);

	
	UE_LOG(LogTemp, Log, TEXT("Room Added! ID: %d, Name: %s"), NewRoom.RoomID, *NewRoom.RoomName);
}