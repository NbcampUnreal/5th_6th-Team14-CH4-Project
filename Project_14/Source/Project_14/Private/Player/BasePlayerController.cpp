// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BasePlayerController.h"

#include "Chatting/ChatInput.h"
#include "GameFramework/PlayerState.h"
#include "GameManager/BaseGameStateBase.h"
#include "GameManager/ProjectGameInstance.h"

ABasePlayerController::ABasePlayerController()
{
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	if (UProjectGameInstance* GI = Cast<UProjectGameInstance>(GetGameInstance()))
	{
		FString PlayerName = GI->PlayerName;
		if (!PlayerName.IsEmpty())
		{
			ServerRPC_SetPlayerName(PlayerName);
		}
	}
	if (ChatInputWidgetClass)
	{
		ChatInputWidgetInstance = CreateWidget<UChatInput>(this,ChatInputWidgetClass);
		if (ChatInputWidgetInstance)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	
}

void ABasePlayerController::ServerRPC_SetPlayerName_Implementation(const FString& Name)
{
	if (PlayerState)
	{
		PlayerState->SetPlayerName(Name);
		UE_LOG(LogTemp,Log,TEXT("Player name: %s"),*Name);
	}
}

bool ABasePlayerController::ServerRPC_SetPlayerName_Validate(const FString& Name)
{
	return true;
}


bool ABasePlayerController::ClientRPCPrintChatMessageString_Validate(const FString& SenderName,const FString& InChatMessageString)
{
	return true;
}

bool ABasePlayerController::ServerRPCPrintChatMessageString_Validate(const FString& InChatMessageString)
{
	return true;
}

void ABasePlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& SenderName,const FString& InChatMessageString)
{
	if (ChatInputWidgetInstance)
	{
		ChatInputWidgetInstance->AddMessage(SenderName,InChatMessageString);
	}
}

void ABasePlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	if (InChatMessageString.IsEmpty())
	{
		return;
	}
	ABaseGameStateBase* GS = GetWorld()->GetGameState<ABaseGameStateBase>();

	if (!GS || !PlayerState)
	{
		return;
	}

	FString SenderName = PlayerState->GetPlayerName();

	TArray<APlayerState*> TargetPlayerStates = GS->GetPlayersForChat(this->PlayerState);
	for (APlayerState* TargetPS : TargetPlayerStates)
	{
		if (TargetPS)
		{
			ABasePlayerController* TargetPC = Cast<ABasePlayerController>(TargetPS->GetOwner());
			if (TargetPC)
			{
				TargetPC->ClientRPCPrintChatMessageString(SenderName,InChatMessageString);
			}
		}
	}
}


