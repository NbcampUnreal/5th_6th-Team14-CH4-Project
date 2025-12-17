// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Server/ServerTypes.h"
#include "UW_LobbyWaitingRoom.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS()
class PROJECT_14_API UUW_LobbyWaitingRoom : public UUserWidget
{
	GENERATED_BODY()

	public:
	virtual void NativeConstruct() override;

	void UpdateRoomUI(const FRoomInfo& Info);

	protected:
	UFUNCTION()
	void OnStartGameClicked();

	UFUNCTION()
	void OnLeaveClicked();
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	int32 CurrentRoomID = -1;
	FString GameServerIP;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> RoomTitleText;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UVerticalBox> PlayerListVBox;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> StartGameButton;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> LeaveButton;
};
