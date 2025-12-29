// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameManager/BaseGameModeBase.h"
#include "LobbyGameModeBase.generated.h"

struct FRoomInfo;
/**
 * 
 */
UCLASS()
class PROJECT_14_API ALobbyGameModeBase : public ABaseGameModeBase
{
	GENERATED_BODY()


public:
	ALobbyGameModeBase();

	//삭제 예정==============================================
	UPROPERTY(EditAnywhere)
	FString GameServerIP;
	void StartGameForRoom(const FRoomInfo& RoomData);
//=================================================================
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
};
