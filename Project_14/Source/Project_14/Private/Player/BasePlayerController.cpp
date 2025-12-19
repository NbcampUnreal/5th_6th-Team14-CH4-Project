// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BasePlayerController.h"

#include "Chatting/ChatInput.h"
#include "GameFramework/PlayerState.h"
#include "GameManager/BaseGameStateBase.h"

ABasePlayerController::ABasePlayerController()
{
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && ChatInputWidgetClass)
	{
		ChatInputWidgetInstance = CreateWidget<UChatInput>(this,ChatInputWidgetClass);
		if (ChatInputWidgetInstance)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	
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


