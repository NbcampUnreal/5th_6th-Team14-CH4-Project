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
}

void AProjectGameStateBase::StartGame()
{
    if (!HasAuthority()) return;

    if (GameStartTime < 0.f)
    {
        GameStartTime = GetWorld()->GetTimeSeconds();
    }
}
float AProjectGameStateBase::GetPlayTime() const
{
    if (GameStartTime < 0.f)
        return 0.f;

    return GetWorld()->GetTimeSeconds() - GameStartTime;
}

TArray<APlayerState*> AProjectGameStateBase::GetPlayersForChat(APlayerState* SenderPS)
{
    return PlayerArray;
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

void AProjectGameStateBase::OnMapCleared()
{
    if (HasAuthority())
    {
        ClearedMapCount++;
    }

    /*전부 클리어 할때 이로직이 필요함
    if (ClearPlayTime < 0.f)
    {
        ClearPlayTime = GetPlayTime();
    }*/
}

FText AProjectGameStateBase::GetCurrentGuideText() const
{
    if (!LevelGuideDataAsset)
        return FText::GetEmpty();

    for (const FLevelGuideData& Data : LevelGuideDataAsset->GuideDatas)
    {
        if (Data.MapClearIndex == ClearedMapCount)
        {
            return Data.GuideText;
        }
    }

    return FText::GetEmpty();
}

void AProjectGameStateBase::OnRep_SelectedTypes()
{
    OnCharacterTypeSelected.Broadcast();
}

void AProjectGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectGameStateBase, SelectedCharacterTypes);
    DOREPLIFETIME(AProjectGameStateBase, GameStartTime);
    DOREPLIFETIME(AProjectGameStateBase, ClearPlayTime);
}
