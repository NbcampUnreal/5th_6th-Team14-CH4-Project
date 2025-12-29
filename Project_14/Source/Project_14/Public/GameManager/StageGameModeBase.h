// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameModeBase.h"
#include "Server/ServerTypes.h"
#include "StageGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API AStageGameModeBase : public ABaseGameModeBase
{
	GENERATED_BODY()

	public:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category= "Game Event")
	void NotifyGameClear();
	void NotifyPlayerGiveUp(APlayerController* GivingUpPlayer, EGameEndReason Reason);
	void ForceKickAllPlayers();
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Gameplay")
	float KickAllPlayersTime = 300.0f;
	protected:
	FTimerHandle EndGameTimerHandle;

	FString LobbyServerURL = TEXT("127.0.0.1:7777");
	bool bIsGameEnded = false;
};
