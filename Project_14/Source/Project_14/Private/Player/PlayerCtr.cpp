#include "Player/PlayerCtr.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
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