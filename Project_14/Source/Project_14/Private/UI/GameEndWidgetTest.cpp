// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameEndWidgetTest.h"

#include "Player/StagePlayerController.h"

void UGameEndWidgetTest::UpdateEndReason(EGameEndReason Reason)
{
	OnUpdateUI(Reason);
}

void UGameEndWidgetTest::OnReturnToLobbyClicked()
{
	if (AStagePlayerController* SPC = Cast<AStagePlayerController>(GetOwningPlayer()))
	{
		SPC->ClientRPC_ReturnToLobby();
	}
}
