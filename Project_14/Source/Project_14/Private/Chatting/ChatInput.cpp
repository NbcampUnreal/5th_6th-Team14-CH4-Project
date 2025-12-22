// Fill out your copyright notice in the Description page of Project Settings.


#include "Chatting/ChatInput.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Player/BasePlayerController.h"

void UChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput)
	{
		EditableTextBox_ChatInput->SetText(FText::GetEmpty());
		EditableTextBox_ChatInput->SetKeyboardFocus();
	}

	if (ChatScrollBox)
	{
		ChatScrollBox->ScrollToEnd();
	}
}

void UChatInput::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (EditableTextBox_ChatInput)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &UChatInput::OnChatInputTextCommitted);
	}
}

void UChatInput::AddMessage(FString Sender, FString Msg)
{
	if (IsValid(ChatScrollBox) == false)
	{
		return;
	}
	UTextBlock* NewTextBlock = NewObject<UTextBlock>(ChatScrollBox);
	if (IsValid(NewTextBlock) == true)
	{
		FString FinalMsg = FString::Printf(TEXT("[%s]: %s"), *Sender, *Msg);
		NewTextBlock->SetText(FText::FromString(FinalMsg));

		FSlateFontInfo FontInfo = NewTextBlock->GetFont();
		FontInfo.Size = 14;
		NewTextBlock->SetFont(FontInfo);
		NewTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		ChatScrollBox->AddChild(NewTextBlock);
		ChatScrollBox->ScrollToEnd();
	}
}

void UChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (Text.IsEmpty())
		{
			return;
		}
		ABasePlayerController* OwningPlayerController = GetOwningPlayer<ABasePlayerController>();
		if (IsValid(OwningPlayerController) == true)
		{
			OwningPlayerController->ServerRPCPrintChatMessageString(Text.ToString());

			if (EditableTextBox_ChatInput)
			{
				EditableTextBox_ChatInput->SetText(FText::GetEmpty());
			}
		}
	}
}
