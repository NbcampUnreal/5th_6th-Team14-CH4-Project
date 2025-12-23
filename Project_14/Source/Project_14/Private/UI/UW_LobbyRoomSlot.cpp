// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_LobbyRoomSlot.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Lobby/LobbyPlayerController.h"

void UUW_LobbyRoomSlot::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UUW_LobbyRoomSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (JoinButton != nullptr)
	{
		JoinButton->OnClicked.AddDynamic(this, &UUW_LobbyRoomSlot::OnJoinButtonClicked);
		
	}
}

void UUW_LobbyRoomSlot::SetRoomInfo(const FRoomInfo& Info)
{

	if (RoomNameText != nullptr)
	{
		RoomNameText->SetText(FText::FromString(Info.RoomName));
	}
	if (HostNameText != nullptr)
	{
		HostNameText->SetText(FText::FromString(Info.HostName));
	}
	if (PlayerCountText != nullptr)
	{
		FString CountStr = FString::Printf(TEXT("%d / %d"), Info.MemberPlayerStates.Num(), Info.MaxPlayers);
		PlayerCountText->SetText(FText::FromString(CountStr));
	}
	TargetRoomID = Info.RoomID;
}

void UUW_LobbyRoomSlot::OnJoinButtonClicked()
{
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		//PC->Server_JoinRoom(TargetRoomID);
		PC->ServerRPC_JoinRoom(TargetRoomID);
	}
}
