// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/StageGameModeBase.h"
#include "Player/StagePlayerController.h"
#include "GameManager/BaseGameStateBase.h"
#include "GameManager/ProjectGameStateBase.h"
#include "GameManager/StageGameStateBase.h"

void AStageGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[Server] StageGameMode BeginPlay Called."));
	if (AStageGameStateBase* SGS = GetGameState<AStageGameStateBase>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server] Sending Ready Signal to Lobby..."));
		SGS->NotifyLobbyServerGameState(true);
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("[Server] GameState is NULL!"));
	}
}

void AStageGameModeBase::NotifyGameClear()
{
	if (bIsGameEnded)
	{
		return;
	}
	bIsGameEnded = true;
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it;++it)
	{
		if (AStagePlayerController* PC = Cast<AStagePlayerController>(it->Get()))
		{
			PC->ClientRPC_ShowGameEndUI(EGameEndReason::Cleared);
		}
	}
	if (AProjectGameStateBase* PGS = GetGameState<AProjectGameStateBase>())
	{
		//PGS->SendGameResultToLobby(bool bIsCleard, float FloatClearTime, FString StringClearTime, const TArray<APlayerState*>& Players);
	}	
	GetWorldTimerManager().SetTimer(EndGameTimerHandle, this, &AStageGameModeBase::ForceKickAllPlayers, KickAllPlayersTime, false);
}

void AStageGameModeBase::NotifyPlayerGiveUp(APlayerController* GivingUpPlayer, EGameEndReason Reason)
{
	if (AStagePlayerController* GiveUpSPC = Cast<AStagePlayerController>(GivingUpPlayer))
	{
		GiveUpSPC->ClientRPC_ReturnToLobby();
	}
	if (bIsGameEnded)
	{
		return;
	}
	bIsGameEnded = true;
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it;++it){
		AStagePlayerController* MemberSPC = Cast<AStagePlayerController>(it->Get());
		if (MemberSPC && MemberSPC != GivingUpPlayer)
		{
			MemberSPC->ClientRPC_ShowGameEndUI(Reason);
		}
	}
	GetWorldTimerManager().SetTimer(EndGameTimerHandle,this,&AStageGameModeBase::ForceKickAllPlayers,KickAllPlayersTime,false);
}

void AStageGameModeBase::ForceKickAllPlayers()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it;++it)
	{
		if (AStagePlayerController* MemberSPC = Cast<AStagePlayerController>(it->Get()))
		{
			MemberSPC->ClientRPC_ReturnToLobby();
		}
	}
}

void AStageGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	int32 RemainingPlayers = 0;
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it;++it)
	{
		AController* PC = it->Get();
		if (PC && PC != Exiting)
		{
			RemainingPlayers++;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("[Server] Logout Called. Remaining Players (excluding exiting): %d"), RemainingPlayers);
	if (RemainingPlayers == 0)
	{
		
		GetWorldTimerManager().ClearTimer(EndGameTimerHandle);
		bIsGameEnded = false;
		
		FString CurrentMapName = GetWorld()->GetMapName();
		CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		//게임서버 재시작
		UE_LOG(LogTemp, Warning, TEXT("[Server] Server reload"));
		GetWorld()->ServerTravel(CurrentMapName);
		
	}else
	{
		if (GameState)
		{
			for (auto PS: GameState->PlayerArray)
			{
				if (PS)
				{
					UE_LOG(LogTemp, Warning, TEXT("[Server] Remaining Player: %s"), *PS->GetPlayerName());
				}
			}
		}
		if (!bIsGameEnded)
		{
			APlayerController* EPC = Cast<APlayerController>(Exiting);
			NotifyPlayerGiveUp(EPC,EGameEndReason::Forfeited);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("[Server] not reload"));
	}
	
}
