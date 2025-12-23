// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Chatting/ChatInput.h"
#include "GameFramework/PlayerState.h"
#include "GameManager/ProjectGameInstance.h"
#include "Lobby/LobbyGameStateBase.h"
#include "Server/ServerTypes.h"
#include "UI/UW_LobbyLayout.h"
#include "UI/UW_LobbyWaitingRoom.h"

#define NET_LOG(Format, ...) UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), \
HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), \
*FString::Printf(Format, ##__VA_ARGS__))

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	/*if (UProjectGameInstance* GI = Cast<UProjectGameInstance>(GetGameInstance()))
	{
		FString PlayerName = GI->PlayerName;
		if (!PlayerName.IsEmpty())
		{
			Server_SetPlayerName(PlayerName);
		}
	}*/

	if (IsValid(UserWidgetClass) == true)
	{
		UserWidgetInstance = CreateWidget<UUW_LobbyLayout>(this, UserWidgetClass);
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


void ALobbyPlayerController::ClientRPC_ShowLobbyUI_Implementation()
{
	if (WaitingRoomWidgetInstance)
	{
		WaitingRoomWidgetInstance->RemoveFromParent();
		WaitingRoomWidgetInstance = nullptr;
	}
	if (UserWidgetClass)
	{
		if (UserWidgetInstance == nullptr)
		{
			UserWidgetInstance = CreateWidget<UUW_LobbyLayout>(this, UserWidgetClass);
		}
		if (UserWidgetInstance != nullptr && !UserWidgetInstance->IsInViewport())
		{
			UserWidgetInstance->AddToViewport();
			UserWidgetInstance->UpdateRoomList();
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
			ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
			//FRoomInfo& MyRoomInfo = GS->RoomList[RoomID];
			WaitingRoomWidgetInstance->UpdateRoomUI(RoomInfo);
			WaitingRoomWidgetInstance->AddToViewport();
			if (ChatInputWidgetInstance)
			{
				ChatInputWidgetInstance->RemoveFromParent();
				ChatInputWidgetInstance->AddToViewport();
			}
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

	FString CurrentPlayerName = "";
	FString FinalURL = ServerAddress;
	if (PlayerState)
	{
		CurrentPlayerName = PlayerState->GetPlayerName();
	}
	if (!CurrentPlayerName.IsEmpty())
	{
		FinalURL += FString::Printf(TEXT("?Name=%s"),*CurrentPlayerName);
	}
	ClientTravel(ServerAddress, ETravelType::TRAVEL_Absolute);
}

void ALobbyPlayerController::ServerRPC_CreateRoom_Implementation(const FString& RoomName)
{
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();
	if (!GS || !PS)
	{
		return;
	}
	if (GS->FindAvailableServerIndex() == -1)
	{
		return;
	}

	int32 NewRoomID = GS->CreateRoom(PS,RoomName);
	int32 NewRoomIndex = GS->GetRoomIndexByID(NewRoomID);
	FRoomInfo TargetRoom = GS->RoomList[NewRoomIndex];
	if (NewRoomIndex != -1)
	{
		ClientRPC_ShowWaitingRoomUI(TargetRoom);
	}
	
}

bool ALobbyPlayerController::ServerRPC_CreateRoom_Validate(const FString& RoomName)
{
	return true;
}

void ALobbyPlayerController::ServerRPC_StartGame_Implementation()
{
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();
	FRoomInfo* MyRoom = nullptr;
	for (FRoomInfo& RoomInfo : GS->RoomList)
	{
		if (RoomInfo.MemberPlayerStates.Contains(PS))
		{
			MyRoom = &RoomInfo;
			break;
		}
	}
	if (MyRoom)
	{
		if (MyRoom->HostPlayerID != PS->GetPlayerId())
		{
			return;
		}
		GS->SetRoomGameStarting(MyRoom->RoomID);
		if (GS->GameServerList.IsValidIndex(MyRoom->AssingedServerIndex))
		{
			FGameServerInfo ServerInfo = GS->GameServerList[MyRoom->AssingedServerIndex];
			FString ServerURL = FString::Printf(TEXT("%s:%d"),*ServerInfo.IPAddress, ServerInfo.Port);
			for (ALobbyPlayerState* MemberPS : MyRoom->MemberPlayerStates)
			{
				APlayerController* PC = Cast<APlayerController>(MemberPS->GetOwningController());
				PC->ClientTravel(ServerURL,TRAVEL_Absolute);
			}
		}
	}
}

bool ALobbyPlayerController::ServerRPC_StartGame_Validate()
{
	return true;
}

void ALobbyPlayerController::ServerRPC_LeaveRoom_Implementation(int32 RoomID)
{
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();
	if (!GS || !PS)
	{
		return;
	}
	bool bLeaveSuccess = GS->LeaveRoom(PS);
	if (bLeaveSuccess)
	{
		ClientRPC_ShowLobbyUI();
	}
}

bool ALobbyPlayerController::ServerRPC_LeaveRoom_Validate(int32 RoomID)
{
	return true;
}

void ALobbyPlayerController::ServerRPC_JoinRoom_Implementation(int32 RoomID)
{
	//NET_LOG(TEXT("Server_JoinRoom Request Received. RoomIndex: %d"), RoomID);
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();
	if (!GS || !PS)
	{
		//NET_LOG(TEXT("ERROR: GameState or PlayerState is NULL!"));
		return;
	}
	bool bAlreadyInRoom = false;
	for (const FRoomInfo& RoomInfo : GS->RoomList)
	{
		if (RoomInfo.MemberPlayerStates.Contains(PS))
		{
			bAlreadyInRoom = true;
			break;
		}
	}
	if (bAlreadyInRoom)
	{
		//NET_LOG(TEXT("Validation Failed: Player is already in a room."));
		return;
	}
	//NET_LOG(TEXT("Validation Passed. Requesting GameState to JoinRoom..."));

	if (GS->JoinRoom(RoomID,PS))
	{
		//NET_LOG(TEXT("JoinRoom Success! Sending RPC to Client."));
		int32 RealRoomIndex = GS->GetRoomIndexByID(RoomID);
		if (RealRoomIndex != -1)
		{
			ClientRPC_ShowWaitingRoomUI(GS->RoomList[RealRoomIndex]);
		}
	}else
	{
		//NET_LOG(TEXT("JoinRoom Failed in GameState."));
	}
	
}

bool ALobbyPlayerController::ServerRPC_JoinRoom_Validate(int32 RoomID)
{
	return true;
}

