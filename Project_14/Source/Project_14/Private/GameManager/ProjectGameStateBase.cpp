#include "GameManager/ProjectGameStateBase.h"
#include "Net/UnrealNetwork.h"

AProjectGameStateBase::AProjectGameStateBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SelectedCharacterTypes.Empty();
}

void AProjectGameStateBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    PlayTime += DeltaSeconds;
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
    DOREPLIFETIME(AProjectGameStateBase, PlayTime);
}
