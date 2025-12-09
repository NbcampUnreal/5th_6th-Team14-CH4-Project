#include "GameManager/ProjectGameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerCtr.h"

AProjectGameModeBase::AProjectGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = APlayerCtr::StaticClass();
}