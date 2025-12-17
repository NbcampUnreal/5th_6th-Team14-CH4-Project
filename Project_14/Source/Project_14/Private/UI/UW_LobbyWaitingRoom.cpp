// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_LobbyWaitingRoom.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Lobby/LobbyGameStateBase.h"
#include "Lobby/LobbyPlayerController.h"
#include "Server/ServerTypes.h"

void UUW_LobbyWaitingRoom::NativeConstruct()
{
	Super::NativeConstruct();
	if (StartGameButton != nullptr)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UUW_LobbyWaitingRoom::OnStartGameClicked);
		StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LeaveButton != nullptr)
	{
		LeaveButton->OnClicked.AddDynamic(this, &UUW_LobbyWaitingRoom::OnLeaveClicked);
	}
	
}

void UUW_LobbyWaitingRoom::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>())
	{
		for (const FRoomInfo& Room: GS->RoomList)
		{
			if (Room.RoomID == CurrentRoomID)
			{
				UpdateRoomUI(Room);
				break;
			}
		}
	}
}


void UUW_LobbyWaitingRoom::UpdateRoomUI(const FRoomInfo& Info)
{
	CurrentRoomID = Info.RoomID;

	if (RoomTitleText != nullptr)
	{
		RoomTitleText->SetText(FText::FromString(Info.RoomName));
	}
	if (PlayerListVBox != nullptr)
	{
		PlayerListVBox->ClearChildren();

		for (ALobbyPlayerState* PS: Info.MemberPlayerStates)
		{
			if (PS != nullptr)
			{
				UTextBlock* NewText = NewObject<UTextBlock>(this);
				NewText->SetText(FText::FromString(PS->GetPlayerName()));
				NewText->SetJustification(ETextJustify::Center);
			}
		}
	}
	if (StartGameButton != nullptr)
	{
		FString MyName ="";
		if (GetOwningPlayer() && GetOwningPlayer()->PlayerState)
		{
			MyName = GetOwningPlayer()->PlayerState->GetPlayerName();
		}

		if (MyName == Info.HostName)
		{
			StartGameButton->SetVisibility(ESlateVisibility::Visible);
		}else
		{
			StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UUW_LobbyWaitingRoom::OnStartGameClicked()
{
	ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		PC->Server_StartGame(CurrentRoomID);
	}
}

void UUW_LobbyWaitingRoom::OnLeaveClicked()
{
	RemoveFromParent();
}

