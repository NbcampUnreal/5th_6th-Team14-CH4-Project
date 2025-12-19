// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BaseGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API ABaseGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	public:
	virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS);
};
