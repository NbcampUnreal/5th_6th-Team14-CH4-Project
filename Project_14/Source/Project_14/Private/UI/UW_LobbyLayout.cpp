// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_LobbyLayout.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Lobby/LobbyGameStateBase.h"
#include "Lobby/LobbyPlayerController.h"
#include "UI/UW_LobbyRoomSlot.h"

void UUW_LobbyLayout::NativeConstruct()
{
	Super::NativeConstruct();

	if (CreateRoomButton != nullptr)
	{
		CreateRoomButton->OnClicked.AddDynamic(this, &UUW_LobbyLayout::OnCreateButtonClicked);
	}
	if (RefreshButton != nullptr)
	{
		RefreshButton->OnClicked.AddDynamic(this, &UUW_LobbyLayout::OnRefreshButtonClicked);
	}
}

void UUW_LobbyLayout::UpdateRoomList()
{
	if (RoomListScrollBox == nullptr || RoomSlotClass == nullptr)
	{
		return;
	}

	RoomListScrollBox->ClearChildren();

	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	if (GS == nullptr)
	{
		return;
	}

	for (const FRoomInfo& RoomInfo : GS->RoomList)
	{
		if (UUW_LobbyRoomSlot* SlotWidget = CreateWidget<UUW_LobbyRoomSlot>(this,RoomSlotClass))
		{
			SlotWidget->SetRoomInfo(RoomInfo);

			RoomListScrollBox->AddChild(SlotWidget);
		}
	}
}

void UUW_LobbyLayout::OnCreateButtonClicked()
{
	if (RoomNameInput == nullptr)
	{
		return;
	}

	FString RoomName = RoomNameInput->GetText().ToString();
	if (RoomName.IsEmpty())
	{
		return;
	}

	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		FRoomInfo NewRoom;
		NewRoom.RoomName = RoomName;
		//NewRoom.MaxPlayers = 2;
		PC->Server_CreateRoom(NewRoom);
	}
}

void UUW_LobbyLayout::OnRefreshButtonClicked()
{
	UpdateRoomList();
}
