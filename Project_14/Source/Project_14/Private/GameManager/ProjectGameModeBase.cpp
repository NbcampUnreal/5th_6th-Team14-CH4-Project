#include "GameManager/ProjectGameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerCtr.h"
#include "GameManager/ProjectGameStateBase.h"

AProjectGameModeBase::AProjectGameModeBase()
{
    DefaultPawnClass = nullptr;
	PlayerControllerClass = APlayerCtr::StaticClass();
    GameStateClass = AProjectGameStateBase::StaticClass();
}

void AProjectGameModeBase::SpawnSelectedCharacter(APlayerController* PC, ECharacterType Type)
{
    if (!PC)
    {
        return;
    }

    AProjectGameStateBase* GS = GetGameState<AProjectGameStateBase>();
    if (!GS)
    {
        return;
    }

    if (GS->SelectedCharacterTypes.Contains(Type))
    {
        return;
    }
    GS->AddSelectedType(Type);

    AActor* Start = FindPlayerStart(PC);
    if (!Start) return;

    FActorSpawnParameters Params;
    Params.Owner = PC;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    TSubclassOf<APlayerCharacter> CharacterToSpawn = nullptr;
    switch (Type)
    {
    case ECharacterType::StrongPush:
        CharacterToSpawn = StrongPushCharacterBP;
        break;
    case ECharacterType::WeakJump:
        CharacterToSpawn = JumpCharacterBP;
        break;
    default:
        break;
    }

    if (!CharacterToSpawn)
    {
        return;
    }

    APlayerCharacter* NewChar = GetWorld()->SpawnActor<APlayerCharacter>(
        CharacterToSpawn, Start->GetActorTransform(), Params);

    PC->Possess(NewChar);
    NewChar->CharacterType = Type;
}
