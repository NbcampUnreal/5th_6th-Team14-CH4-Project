// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_TitleLayout.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "GameManager/ProjectGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Title/TitlePlayerController.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UUW_TitleLayout::NativeConstruct()
{
	
}

void UUW_TitleLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	FString InputName = PlayerNameEditableText->GetText().ToString().TrimStartAndEnd();
	FString InputIP = ServerIPEditableText->GetText().ToString().TrimStartAndEnd();

	
	if (InputName.IsEmpty() || InputIP.IsEmpty())
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Name or IP is empty!"));
		return;
	}
	UProjectGameInstance* GI = Cast<UProjectGameInstance>(GetGameInstance());
	if (GI != nullptr)
	{
		GI->PlayerName = InputName;
		GI->LobbyServerPublicIP = ServerIPEditableText->GetText().ToString();
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance!"));
		return; 
	}
	ATitlePlayerController* PlayerController = GetOwningPlayer<ATitlePlayerController>();
	if (IsValid(PlayerController) == true)
	{
		//FText ServerIP = GI->LobbyServerPublicIP;
		PlayerController->JoinServer(GI->LobbyServerPublicIP);
	}
}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

