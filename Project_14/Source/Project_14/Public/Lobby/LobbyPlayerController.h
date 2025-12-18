// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Server/ServerTypes.h"
#include "UI/UW_LobbyWaitingRoom.h"
#include "LobbyPlayerController.generated.h"

class UUW_LobbyLayout;
/**
 * 
 */
UCLASS()
class PROJECT_14_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;


	//Client RPC
	UFUNCTION(Client, Reliable)
	void ClientRPC_MoveToGameServer(const FString& ServerAddress);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowWaitingRoomUI(const FRoomInfo& RoomInfo);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowLobbyUI();

	//Server RPC
	UFUNCTION(Server, Reliable,WithValidation)
	void Server_CreateRoom(const FRoomInfo& NewRoomInfo);
	
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_JoinRoom(int32 RoomID);
	
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_StartGame(int32 RoomID);

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_SetPlayerName(const FString& Name);

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_LeaveRoom(int32 RoomID);	
	
	
	//etc
	void RequestCreateRoom(FString RoomName);

protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Lobby UI", meta = (AllowPrivateAccess))
	TSubclassOf<UUW_LobbyLayout> UserWidgetClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Lobby UI", meta = (AllowPrivateAccess))
	TSubclassOf<UUW_LobbyWaitingRoom> WaitingRoomWidgetClass;

	
private:
	

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Lobby UI", meta = (AllowPrivateAccess))
	TObjectPtr<UUW_LobbyLayout> UserWidgetInstance;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Lobby UI", meta = (AllowPrivateAccess))
	TObjectPtr<UUW_LobbyWaitingRoom> WaitingRoomWidgetInstance;

};
