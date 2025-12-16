// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Lobby/LobbyGameStateBase.h"
#include "Server/ServerTypes.h"


void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	if (IsValid(UserWidgetClass) == true)
	{
		UserWidgetInstance = CreateWidget<UUserWidget>(this, UserWidgetClass);
		if (IsValid(UserWidgetInstance) == true)
		{
			UserWidgetInstance->AddToViewport();
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UserWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}

void ALobbyPlayerController::ClientRPC_MoveToGameServer_Implementation(const FString& ServerAddress)
{
	UE_LOG(LogTemp, Log, TEXT("Server ordered to move: %s"), *ServerAddress);

	ClientTravel(ServerAddress, ETravelType::TRAVEL_Absolute);
}


void ALobbyPlayerController::Server_CreateRoom_Implementation(const FRoomInfo& NewRoomInfo)
{
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	if (IsValid(GS) == true)
	{
		FRoomInfo FinalRoomInfo = NewRoomInfo;
		FinalRoomInfo.RoomID = GS->RoomList.Num() + 1;
		FinalRoomInfo.HostName = PlayerState->GetPlayerName();
		GS->AddRoom(FinalRoomInfo);

		UE_LOG(LogTemp, Log, TEXT("Server: Creat Room {%s} by %s"),*FinalRoomInfo.RoomName, *FinalRoomInfo.HostName);

	}
}

bool ALobbyPlayerController::Server_CreateRoom_Validate(const FRoomInfo& NewRoomInfo)
{
	if (NewRoomInfo.RoomName.IsEmpty())
	{
		return false;
	}
	return true;
}



void ALobbyPlayerController::RequestCreateRoom(FString RoomName)
{
	FRoomInfo NewRoom;
	NewRoom.RoomName = RoomName;
	NewRoom.MaxPlayers = 2;
	NewRoom.HostName = PlayerState->GetPlayerName();

	Server_CreateRoom(NewRoom);

}

void ALobbyPlayerController::Server_StartGame_Implementation(int32 RoomID,const FString& GameServerIP)
{
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	if (IsValid(GS) == false)
	{
		return;
	}
	FRoomInfo* TargetRoom = nullptr;
	for (FRoomInfo& Room : GS->RoomList)
	{
		if (Room.RoomID == RoomID)
		{
			TargetRoom = &Room;
			break;
		}
	}
	if (TargetRoom != nullptr)
	{
		if (TargetRoom->HostName != PlayerState->GetPlayerName())
		{
			return;
		}
		FString ConnectURL = FString::Printf(TEXT("%s:%d"),*GameServerIP,TargetRoom->GameServerPort);
		for (ALobbyPlayerState* MemberPS : TargetRoom->MemberPlayerStates)
		{
			if (MemberPS != nullptr)
			{
				ALobbyPlayerController* MemberPC = Cast<ALobbyPlayerController>(MemberPS->GetOwner());
				if (MemberPC != nullptr)
				{
					MemberPC->ClientRPC_MoveToGameServer(ConnectURL);
				}
			}
		}
	}
	
}

bool ALobbyPlayerController::Server_StartGame_Validate(int32 RoomID,const FString& GameServerIP)
{
	return true;
}

void ALobbyPlayerController::Server_JoinRoom_Implementation(int32 RoomID)
{
	
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	if (IsValid(GS) == false)
	{
		return;
	}
	FRoomInfo* TargetRoom = nullptr;
	for (FRoomInfo& Room : GS->RoomList)
	{
		if (Room.RoomID == RoomID)
		{
			TargetRoom = &Room;
			break;
		}
	}
	if (TargetRoom != nullptr && TargetRoom->CurrentPlayers < TargetRoom->MaxPlayers )
	{
		ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();
		TargetRoom->MemberPlayerStates.Add(PS);
		TargetRoom->CurrentPlayers++;

		//need waiting room UI client RPC
		//ClientRPC_ShowWaitingRoomUI(TargetRoom->RoomName);
	}
}

bool ALobbyPlayerController::Server_JoinRoom_Validate(int32 RoomID)
{
	return true;
}
