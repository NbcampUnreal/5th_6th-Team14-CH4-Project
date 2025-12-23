// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"


class UChatInput;
class UUserWidget;
/**
 * 
 */
UCLASS()
class PROJECT_14_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
	ABasePlayerController();
	
	virtual void BeginPlay() override;

	//Chatting
	UFUNCTION(Client, Reliable,WithValidation)
	void ClientRPCPrintChatMessageString(const FString& SenderName,const FString& InChatMessageString);
	UFUNCTION(Server, Reliable,WithValidation)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	//Set Player Info
	UFUNCTION(Server,Reliable,WithValidation)
	void ServerRPC_SetPlayerName(const FString& Name);
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UChatInput> ChatInputWidgetInstance;
	

	FString ChatMessageString;
};
