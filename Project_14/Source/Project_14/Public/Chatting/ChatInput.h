// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatInput.generated.h"

class UScrollBox;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class PROJECT_14_API UChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	void AddMessage(FString Sender, FString Msg);

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
};
