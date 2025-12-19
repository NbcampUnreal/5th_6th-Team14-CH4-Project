// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/BaseGameStateBase.h"

TArray<APlayerState*> ABaseGameStateBase::GetPlayersForChat(APlayerState* SenderPS)
{
	return PlayerArray;
}
