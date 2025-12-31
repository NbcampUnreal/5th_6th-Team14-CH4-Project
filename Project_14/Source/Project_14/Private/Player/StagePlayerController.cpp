// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/StagePlayerController.h"

#include "GameManager/ProjectGameInstance.h"
#include "GameManager/StageGameModeBase.h"
#include "Server/ServerConfigSettings.h" 

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
	/*const UServerConfigSettings* Settings = UServerConfigSettings::Get();
	FString LobbyIP = Settings->LobbyServerPublicIP + ":" + FString::FromInt(Settings->LobbyServerPort);*/
	UProjectGameInstance* GI = Cast<UProjectGameInstance>(GetGameInstance());
	ClientTravel(GI->LobbyServerPublicIP, ETravelType::TRAVEL_Absolute);
}
