#include "GameManager/ProjectGameState.h"
#include "Net/UnrealNetwork.h"

AProjectGameState::AProjectGameState()
{
    bReplicates = true;
    SelectedCharacterTypes.Empty();
}

void AProjectGameState::AddSelectedType(ECharacterType Type)
{
    if (HasAuthority())
    {
        if (!SelectedCharacterTypes.Contains(Type))
        {
            SelectedCharacterTypes.Add(Type);
        }
    }
}

void AProjectGameState::OnRep_SelectedTypes()
{
    OnCharacterTypeSelected.Broadcast();
}

void AProjectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectGameState, SelectedCharacterTypes);
}