// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/StagePlayerController.h"
#include "GameManager/StageGameModeBase.h"

void AStagePlayerController::ServerRPC_GiveUp_Implementation()
{
	if (AStageGameModeBase* SGM = GetWorld()->GetAuthGameMode<AStageGameModeBase>())
	{
		SGM->NotifyPlayerGiveUp(this,EGameEndReason::GivenUp);
	}
}

bool AStagePlayerController::ServerRPC_GiveUp_Validate()
{
	return true;
}

void AStagePlayerController::ClientRPC_ShowGameEndUI_Implementation(EGameEndReason Reason)
{
	OnShowGameEndUI(Reason);
	UE_LOG(LogTemp, Log, TEXT("Show Game End UI: %d"), (int32)Reason);
}

void AStagePlayerController::ClientRPC_ReturnToLobby_Implementation()
{
	FString LobbyIP = TEXT("127.0.0.1:7777"); 
	ClientTravel(LobbyIP, ETravelType::TRAVEL_Absolute);
}
