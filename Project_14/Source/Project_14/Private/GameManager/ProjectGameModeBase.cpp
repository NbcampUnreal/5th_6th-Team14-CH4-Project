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

    if (PC->GetPawn())
        return;

    AProjectGameStateBase* GS = GetGameState<AProjectGameStateBase>();
    if (!GS)
    {
        return;
    }

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

void AProjectGameModeBase::TryStartGame()
{
    AProjectGameStateBase* GS = GetGameState<AProjectGameStateBase>();
    if (!GS) return;

    const int32 PlayerCount = GameState->PlayerArray.Num();
    const int32 SelectedCount = GS->SelectedCharacterTypes.Num();

    if (SelectedCount == PlayerCount)
    {
        GS->StartGame(); 
    }
}
