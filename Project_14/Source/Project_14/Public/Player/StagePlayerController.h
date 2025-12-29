// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "Server/ServerTypes.h"
#include "StagePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API AStagePlayerController : public ABasePlayerController
{
	GENERATED_BODY()

	public:
	UFUNCTION(Server, Reliable,BlueprintCallable, WithValidation)
	void ServerRPC_GiveUp();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Game Event")
	void OnShowGameEndUI(EGameEndReason Reason);
	
	UFUNCTION(Client,Reliable)
	void ClientRPC_ShowGameEndUI(EGameEndReason Reason);

	UFUNCTION(Client,Reliable, BlueprintCallable)
	void ClientRPC_ReturnToLobby();

	
};
