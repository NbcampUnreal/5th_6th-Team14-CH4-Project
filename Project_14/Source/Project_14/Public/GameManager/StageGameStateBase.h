// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameStateBase.h"
#include "StageGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API AStageGameStateBase : public ABaseGameStateBase
{
	GENERATED_BODY()

	public:
	void NotifyLobbyServerGameState(bool bIsIdle);
	virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS) override;
};
