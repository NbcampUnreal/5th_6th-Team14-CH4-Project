#include "Player/PlayerCtr.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UI/MH/InGameHUDWidget.h"
#include "GameManager/ProjectGameModeBase.h"

APlayerCtr::APlayerCtr()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	MouseInteractAction(nullptr)
{
}

void APlayerCtr::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	if (!IsLocalController())
		return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewA = GetWorld()->SpawnActor<AActor>(
		PreviewCharacterA,
		FVector(0, 0, -10000),
		FRotator::ZeroRotator,
		Params
	);

	PreviewB = GetWorld()->SpawnActor<AActor>(
		PreviewCharacterB,
		FVector(300, 0, -10000),
		FRotator::ZeroRotator,
		Params
	);

	// 처음엔 A만 보여주기
	if (PreviewA) PreviewA->SetActorHiddenInGame(false);
	if (PreviewB) PreviewB->SetActorHiddenInGame(false);
}

void APlayerCtr::Server_SelectCharacterType_Implementation(ECharacterType Type)
{
	AProjectGameModeBase* GM = Cast<AProjectGameModeBase>(
		UGameplayStatics::GetGameMode(this));

	if (GM)
	{
		GM->SpawnSelectedCharacter(this, Type);
	}

	Client_SetGameInputMode();
}

void APlayerCtr::Client_SetGameInputMode_Implementation()
{
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void APlayerCtr::DestroyAllPreviewCharacters()
{
	if (PreviewA)
	{
		PreviewA->Destroy();
		PreviewA = nullptr;
	}

	if (PreviewB)
	{
		PreviewB->Destroy();
		PreviewB = nullptr;
	}
}

void APlayerCtr::CreateInGameHUD()
{
	if (!IsLocalController())
		return;

	if (InGameHUD)
		return;

	if (InGameHUDClass)
	{
		InGameHUD = CreateWidget<UInGameHUDWidget>(this, InGameHUDClass);
		if (InGameHUD)
		{
			InGameHUD->AddToViewport();
		}
	}
}

void APlayerCtr::RemoveInGameHUD()
{
	if (InGameHUD)
	{
		InGameHUD->RemoveFromParent();
		InGameHUD = nullptr;
	}
}