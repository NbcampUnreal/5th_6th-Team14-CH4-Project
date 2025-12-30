// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/StageGameStateBase.h"
#include "Server/ServerConfigSettings.h"


void AStageGameStateBase::NotifyLobbyServerGameState(bool bIsIdle)
{
	if (HasAuthority())
	{
		int32 MyPort = GetWorld()->URL.Port;
		SendServerStatusToLobby(MyPort,bIsIdle);
		FString StateStr = bIsIdle ? TEXT("Idle (Ready)") : TEXT("Busy (Playing)");
		UE_LOG(LogTemp, Warning, TEXT("[StageGS] Reported Status to Lobby: %s"), *StateStr);
	}
}

TArray<APlayerState*> AStageGameStateBase::GetPlayersForChat(APlayerState* SenderPS)
{
	return Super::GetPlayersForChat(SenderPS);
}
