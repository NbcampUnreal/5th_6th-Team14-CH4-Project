// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacter.h"
#include "Player/StagePlayerController.h"
#include "PlayerCtr.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECT_14_API APlayerCtr : public AStagePlayerController
{
	GENERATED_BODY()
	
public:

	APlayerCtr();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MouseInteractAction;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SelectCharacterType(ECharacterType Type);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_SetGameInputMode();

	UPROPERTY(EditDefaultsOnly, Category = "UI Preview")
	TSubclassOf<AActor> PreviewCharacterA;

	UPROPERTY(EditDefaultsOnly, Category = "UI Preview")
	TSubclassOf<AActor> PreviewCharacterB;

	UPROPERTY()
	AActor* PreviewA;

	UPROPERTY()
	AActor* PreviewB;

	UFUNCTION(BlueprintCallable)
	void DestroyAllPreviewCharacters();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UInGameHUDWidget> InGameHUDClass;

	UPROPERTY()
	UInGameHUDWidget* InGameHUD;

	UFUNCTION(BlueprintCallable)
	void CreateInGameHUD();

	UFUNCTION(BlueprintCallable)
	void RemoveInGameHUD();

protected:
	virtual void BeginPlay() override;

};
