// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Server/ServerTypes.h"
#include "GameEndWidgetTest.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API UGameEndWidgetTest : public UUserWidget
{
	GENERATED_BODY()
	public:
	void UpdateEndReason(EGameEndReason Reason);

	protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateUI(EGameEndReason Reason);

	UFUNCTION(BlueprintCallable)
	void OnReturnToLobbyClicked();
};
