#include "GameManager/ProjectGameStateBase.h"
#include "Net/UnrealNetwork.h"

AProjectGameStateBase::AProjectGameStateBase()
{
    bReplicates = true;
    SelectedCharacterTypes.Empty();
}

void AProjectGameStateBase::AddSelectedType(ECharacterType Type)
{
    if (HasAuthority())
    {
        if (!SelectedCharacterTypes.Contains(Type))
        {
            SelectedCharacterTypes.Add(Type);
        }
    }
}

void AProjectGameStateBase::OnRep_SelectedTypes()
{
    OnCharacterTypeSelected.Broadcast();
}

void AProjectGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectGameStateBase, SelectedCharacterTypes);
}
