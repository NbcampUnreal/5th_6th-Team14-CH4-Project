// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Server/ServerTypes.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
	virtual void BeginPlay() override;
	UFUNCTION(Client, Reliable)
	void ClientRPC_MoveToGameServer(const FString& ServerAddress);

	UFUNCTION(Server, Reliable,WithValidation)
	void Server_CreateRoom(const FRoomInfo& NewRoomInfo);
	void RequestCreateRoom(FString RoomName);

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_JoinRoom(int32 RoomID);

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_StartGame(int32 RoomID,const FString& GameServerIP);

private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Lobby UI", meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UserWidgetClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Lobby UI", meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UserWidgetInstance;
	
};
