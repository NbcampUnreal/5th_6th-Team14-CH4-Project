#include "Lobby/LobbyGameStateBase.h"

#include "JsonObjectConverter.h"
#include "Server/ServerTypes.h"
#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyPlayerState.h"
#include "Net/UnrealNetwork.h" 
#include "Server/ServerConfigSettings.h"


#define NET_LOG(Format, ...) UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), \
HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), \
*FString::Printf(Format, ##__VA_ARGS__))

void ALobbyGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		InitializeServerList();
		StartHttpListener(8081);
		if (LeaderBoard.Num() == 0)
		{
			for (int32 i = 0; i <10; ++i)
			{
				LeaderBoard.Add(FRankRecord());
			}
		}
	}
}



void ALobbyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALobbyGameStateBase, RoomList);
	DOREPLIFETIME(ALobbyGameStateBase, GameServerList);
	
}


TArray<APlayerState*> ALobbyGameStateBase::GetPlayersForChat(APlayerState* SenderPS)
{
	TArray<APlayerState*> TargetPlayers;
	ALobbyPlayerState* LobbySenderPS = Cast<ALobbyPlayerState>(SenderPS);
	if (IsValid(LobbySenderPS) == false)
	{
		return TargetPlayers;
	}
	if (LobbyUsers.Contains(SenderPS) == true)
	{
		return LobbyUsers;
	}
	for (FRoomInfo room : RoomList)
	{
		if (room.MemberPlayerStates.Contains(LobbySenderPS))
		{
			for (auto MemberPS : room.MemberPlayerStates)
			{
				TargetPlayers.Add(MemberPS);
			}
		}
	}
	
	return TargetPlayers;
	
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

void ALobbyGameStateBase::AddLobbyUser(APlayerState* NewUser)
{
	LobbyUsers.AddUnique(NewUser);
}

void ALobbyGameStateBase::ProcessPlayerLogout(APlayerState* ExitingPS)
{
	if (LobbyUsers.Contains(ExitingPS) == true)
	{
		LobbyUsers.Remove(ExitingPS);
		return;
	}
	ALobbyPlayerState* LobbyExitingPS = Cast<ALobbyPlayerState>(ExitingPS);
	if (IsValid(LobbyExitingPS) == false)
	{
		return;
	}
	for (int32 i = RoomList.Num() - 1; i >= 0; --i)
	{
		if (RoomList[i].MemberPlayerStates.Contains(LobbyExitingPS) == true)
		{
			RoomList[i].MemberPlayerStates.Remove(LobbyExitingPS);

			if (RoomList[i].MemberPlayerStates.Num() == 0)
			{
				int32 ServerIndex = RoomList[i].AssingedServerIndex;
				if (RoomList[i].bIsGameStarting == false)
				{
					SetServerBusyStatus(ServerIndex, false);
				}
				RoomList.RemoveAt(i);
			}
			if (OnRoomListUpdated.IsBound())
			{
				OnRoomListUpdated.Broadcast();
			}
			break;
		}
	}
}

int32 ALobbyGameStateBase::GetRoomIndexByID(int32 InRoomID)
{
	for (int32 i = 0; i < RoomList.Num(); ++i)
	{
		if (RoomList[i].RoomID == InRoomID)
		{
			return i;
		}
	}
	return -1;
}

int32 ALobbyGameStateBase::CreateRoom(ALobbyPlayerState* Host, FString RoomName)
{
	int32 ServerIndex = FindAvailableServerIndex();
	if (ServerIndex == -1)
	{
		return -1;
	}
	SetServerBusyStatus(ServerIndex, true);

	FRoomInfo NewRoom;
	NewRoom.RoomName = RoomName;
	NewRoom.HostPlayerID = Host->GetPlayerId();
	NewRoom.AssingedServerIndex = ServerIndex;
	if (GameServerList.IsValidIndex(ServerIndex))
	{
		NewRoom.GameServerIP = GameServerList[ServerIndex].IPAddress;
		NewRoom.GameServerPort = GameServerList[ServerIndex].Port;
	}
	NewRoom.MemberPlayerStates.AddUnique(Host);
	NewRoom.HostName = Host->GetPlayerName();
	NewRoom.MaxPlayers = 2;
	NewRoom.RoomID = RoomIDCounter++;

	RoomList.Add(NewRoom);
	
	LobbyUsers.Remove(Host);
	

	if (OnRoomListUpdated.IsBound())
	{
		OnRoomListUpdated.Broadcast();
	}
	UE_LOG(LogTemp, Log, TEXT("[Lobby] Room Created: ID %d, Server Port: %d"), NewRoom.RoomID, NewRoom.GameServerPort);
	return NewRoom.RoomID;
}

bool ALobbyGameStateBase::JoinRoom(int32 RoomID, ALobbyPlayerState* Joiner)
{
	//NET_LOG(TEXT("[SERVER] GameState::JoinRoom Called. Index: %d"),RoomID);
	int32 RealRoomIndex = GetRoomIndexByID(RoomID);
	
	if (RealRoomIndex != -1)
	{
		if (RoomList[RealRoomIndex].MaxPlayers <= RoomList[RealRoomIndex].MemberPlayerStates.Num())
		{
			//NET_LOG(TEXT("[SERVER] GameState::JoinRoom Failed. Full Room"));

			return false;
		}
		RoomList[RealRoomIndex].MemberPlayerStates.AddUnique(Joiner);
		LobbyUsers.Remove(Joiner);

		if (OnRoomListUpdated.IsBound())
		{
			OnRoomListUpdated.Broadcast();
		}
		//NET_LOG(TEXT("[SERVER] GameState::JoinRoom Success. Member Count: %d"),RoomList[RoomID].MemberPlayerStates.Num());

		
		return true;
	}
	//NET_LOG(TEXT("[SERVER] GameState::JoinRoom Failed. Invalid Index."));

	return false;
}

bool ALobbyGameStateBase::LeaveRoom(ALobbyPlayerState* Leaver)
{
	if (IsValid(Leaver) == false)
	{
		return false;
	}
	for (int32 i = RoomList.Num() - 1; i >= 0; --i)
	{
		if (RoomList[i].MemberPlayerStates.Contains(Leaver) == true)
		{
			FRoomInfo& TargetRoom = RoomList[i];
			TargetRoom.MemberPlayerStates.Remove(Leaver);
			LobbyUsers.AddUnique(Leaver);
			if (TargetRoom.MemberPlayerStates.Num() == 0)
			{
				int32 ServerID = RoomList[i].AssingedServerIndex;
				SetServerBusyStatus(ServerID, false);
				RoomList.RemoveAt(i);
			}else if (TargetRoom.HostPlayerID == Leaver->GetPlayerId())
			{
				for (ALobbyPlayerState* MemberPS: TargetRoom.MemberPlayerStates)
				{
					LobbyUsers.AddUnique(MemberPS);
					if (ALobbyPlayerController* MemberPC = Cast<ALobbyPlayerController>(MemberPS->GetOwner()))
					{
						MemberPC->ClientRPC_ShowLobbyUI();
					}
				}
				SetServerBusyStatus(TargetRoom.AssingedServerIndex, false);
				RoomList.RemoveAt(i);
			}
			if (OnRoomListUpdated.IsBound())
			{
				OnRoomListUpdated.Broadcast();
			}
			return true;
		}
	}
	return false;
}

void ALobbyGameStateBase::SetRoomGameStarting(int32 RoomID)
{
	int32 RoomIndex = GetRoomIndexByID(RoomID);
	if (RoomIndex != -1)
	{
		RoomList[RoomIndex].bIsGameStarting = true;
	}
}

int32 ALobbyGameStateBase::FindAvailableServerIndex()
{
	for (int32 i =0; i <GameServerList.Num(); ++i)
	{
		UE_LOG(LogTemp, Log, TEXT("[FindServer] Index: %d, Port: %d, bIsBusy: %d"), i, GameServerList[i].Port, GameServerList[i].bIsBusy);
		if (!GameServerList[i].bIsBusy)
		{
			return i;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[FindServer] No available servers found!"));
	return -1;
}

void ALobbyGameStateBase::SetServerBusyStatus(int32 ServerID, bool bBusy)
{
	if (GameServerList.IsValidIndex(ServerID) == true)
	{
		GameServerList[ServerID].bIsBusy = bBusy;
	}
}

void ALobbyGameStateBase::InitializeServerList()
{
	if (HasAuthority())
	{
		const UServerConfigSettings* Settings = UServerConfigSettings::Get();
		GameServerList.Empty();
		for (int32 Port: Settings->GameServerPorts)
		{
			GameServerList.Add(FGameServerInfo(Settings->GameServerPublicIP,Port));
		}
		UE_LOG(LogTemp, Log, TEXT("[InitializeServerList]"));
	}
}

void ALobbyGameStateBase::OnRep_RoomList()
{
	if (OnRoomListUpdated.IsBound())
	{
		OnRoomListUpdated.Broadcast();
	}
}

void ALobbyGameStateBase::OnServerStatusReported(int32 ServerPort, bool bIsIdle)
{
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Received Report - Port: %d, IsIdle: %d"), ServerPort, bIsIdle);
	
	bool bFound = false;
	for (int32 i = 0; i < GameServerList.Num(); ++i)
	{
		if (GameServerList[i].Port == ServerPort)
		{
			GameServerList[i].bIsBusy = !bIsIdle;
			UE_LOG(LogTemp, Warning, TEXT("[Lobby] Server [%d] Status Updated -> Busy: %d"), ServerPort, GameServerList[i].bIsBusy);
			bFound = true;
			if (OnRoomListUpdated.IsBound())
			{
				OnRoomListUpdated.Broadcast();
			}
			break;
		}
	}
	if (!bFound)
	{
		UE_LOG(LogTemp, Error, TEXT("[Lobby] Failed to find server with port: %d"), ServerPort);
	}
}

void ALobbyGameStateBase::OnLeaderBoardUpdated(FGameResultReport RecievedReport)
{
	Super::OnLeaderBoardUpdated(RecievedReport);
	
	TArray<FString> PlayerNames;
	float ClearTime = RecievedReport.num_clear_time;
	for (FString PlayerName : RecievedReport.player_names)
	{
		PlayerNames.Add(PlayerName);
	}
	PlayerNames.Sort();
	FRankRecord NewRank(PlayerNames, ClearTime);
	LeaderBoard.Add(NewRank);
	LeaderBoard.Sort([](const FRankRecord& L, const FRankRecord& R)
	{
		return L.ClearTime < R.ClearTime;
	});
	if (LeaderBoard.Num() > 10)
	{
		LeaderBoard.RemoveAt(10,LeaderBoard.Num() - 10);
	}
}

void ALobbyGameStateBase::StartHttpListener(int32 Port)
{
	Super::StartHttpListener(Port);

	if (!HasAuthority()) return;

	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(Port);

	if (!HttpRouter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Lobby] Failed to get Router for Port %d"), Port);
		return;
	}

	FHttpRequestHandler GameResultHandler = FHttpRequestHandler::CreateLambda(
	   [this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) -> bool
	   {
		  return this->HandleGameResultRequest(Request, OnComplete);
	   }
	);

	HttpRouter->BindRoute(FHttpPath(TEXT("/api/game_result")), EHttpServerRequestVerbs::VERB_POST, GameResultHandler);

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Additional Route Bound: /api/game_result"));
}

bool ALobbyGameStateBase::HandleGameResultRequest(const FHttpServerRequest& Request,
	const FHttpResultCallback& OnComplete)
{
	FString BodyStr;
	const TArray<uint8>& BodyData = Request.Body;
	BodyStr.Append((const char*)BodyData.GetData(), BodyData.Num());

	FGameResultReport RecievedReport;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(BodyStr,&RecievedReport,0,0))
	{
		AsyncTask(ENamedThreads::GameThread, [this,RecievedReport]()
		{
			OnLeaderBoardUpdated(RecievedReport);
		});
		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("Success"), TEXT("text/plain"));
		OnComplete(MoveTemp(Response));
		return true;
	}
	
	return false;
}
