#include "GameManager/ProjectGameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerCtr.h"
#include "GameManager/ProjectGameState.h"

AProjectGameModeBase::AProjectGameModeBase()
{
    DefaultPawnClass = nullptr;
	PlayerControllerClass = APlayerCtr::StaticClass();
    //GameStateClass = AProjectGameState::StaticClass();
}

void AProjectGameModeBase::SpawnSelectedCharacter(APlayerController* PC, ECharacterType Type)
{
    if (!PC || !PlayerCharacterClass)
    {
        return;
    }

   /* AProjectGameState* GS = GetGameState<AProjectGameState>();
    if (!GS)
    {
        return;
    }

    if (GS->SelectedCharacterTypes.Contains(Type))
    {
        return;
    }
    GS->AddSelectedType(Type);*/

    AActor* Start = FindPlayerStart(PC);
    if (!Start) return;

    FActorSpawnParameters Params;
    Params.Owner = PC;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APlayerCharacter* NewChar =GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterClass, Start->GetActorTransform(),Params);

    if (!NewChar)
    {
        return;
    }

    PC->Possess(NewChar);
    NewChar->CharacterType = Type;
}
