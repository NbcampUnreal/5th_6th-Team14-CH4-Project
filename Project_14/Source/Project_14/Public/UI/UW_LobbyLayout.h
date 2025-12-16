// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyLayout.generated.h"

class UButton;
class UEditableText;
class UScrollBox;
class UUW_LobbyRoomSlot;
/**
 * 
 */
UCLASS()
class PROJECT_14_API UUW_LobbyLayout : public UUserWidget
{
	GENERATED_BODY()

	public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateRoomList();

	protected:
	UFUNCTION()
	void OnCreateButtonClicked();

	UFUNCTION()
	void OnRefreshButtonClicked();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby UI")
	TSubclassOf<UUW_LobbyRoomSlot> RoomSlotClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> RoomNameInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> CreateRoomButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> RefreshButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UScrollBox> RoomListScrollBox;
};
