// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Server/ServerTypes.h"
#include "UW_LobbyRoomSlot.generated.h"

/**
 * 
 */
class UTextBlock;
class UButton;

UCLASS()
class PROJECT_14_API UUW_LobbyRoomSlot : public UUserWidget
{
	GENERATED_BODY()

	public:
	virtual void NativeConstruct() override;

	void SetRoomInfo(const FRoomInfo& Info);

	protected:
	UFUNCTION()
	void OnJoinButtonClicked();

private:
	int32 TargetRoomID = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> RoomNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> HostNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> JoinButton;
};
