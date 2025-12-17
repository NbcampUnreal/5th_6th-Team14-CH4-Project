// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Lobby/LobbyGameStateBase.h"
#include "Server/ServerTypes.h"
#include "UI/UW_LobbyWaitingRoom.h"


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

void ALobbyPlayerController::ClientRPC_ShowWaitingRoomUI_Implementation(const FRoomInfo& RoomInfo)
{
	if (UserWidgetInstance != nullptr)
	{
		UserWidgetInstance->RemoveFromParent();
	}
	if (WaitingRoomWidgetClass !=nullptr)
	{
		if (WaitingRoomWidgetInstance == nullptr)
		{
			WaitingRoomWidgetInstance = CreateWidget<UUW_LobbyWaitingRoom>(this, WaitingRoomWidgetClass);
		}
		if (WaitingRoomWidgetInstance != nullptr)
		{
			WaitingRoomWidgetInstance->UpdateRoomUI(RoomInfo);
			WaitingRoomWidgetInstance->AddToViewport();
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(WaitingRoomWidgetInstance->GetCachedWidget());
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
		FinalRoomInfo.GameServerIP = TEXT("127.0.0.1");
		FinalRoomInfo.GameServerPort = GS->RoomList.Num() + 7778;
		

		UE_LOG(LogTemp, Log, TEXT("Server: Creat Room {%s} by %s"),*FinalRoomInfo.RoomName, *FinalRoomInfo.HostName);

		if (ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>())
		{
			FinalRoomInfo.MemberPlayerStates.Add(PS);
			FinalRoomInfo.CurrentPlayers = 1;
		}
		GS->AddRoom(FinalRoomInfo);
		ClientRPC_ShowWaitingRoomUI(FinalRoomInfo);
		
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

void ALobbyPlayerController::Server_StartGame_Implementation(int32 RoomID)
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
		FString ConnectURL = FString::Printf(TEXT("%s:%d"),*(TargetRoom->GameServerIP),TargetRoom->GameServerPort);
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

bool ALobbyPlayerController::Server_StartGame_Validate(int32 RoomID)
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
		if (ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>())
		{
			TargetRoom->MemberPlayerStates.Add(PS);
			TargetRoom->CurrentPlayers++;
			ClientRPC_ShowWaitingRoomUI(*TargetRoom);	
		}
		
		
	}
}

bool ALobbyPlayerController::Server_JoinRoom_Validate(int32 RoomID)
{
	return true;
}
