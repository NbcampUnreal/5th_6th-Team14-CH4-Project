#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerCharacter.h"
#include "GameManager/StageGameStateBase.h"
#include "UI/MH/Level_DataAsset.h"
#include "ProjectGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterTypeSelected);


UCLASS()
class PROJECT_14_API AProjectGameStateBase : public AStageGameStateBase
{
	GENERATED_BODY()
	
public:
    AProjectGameStateBase();

    virtual void Tick(float DeltaTime) override;
    virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS) override;

    UPROPERTY(ReplicatedUsing = OnRep_SelectedTypes)
    TArray<ECharacterType> SelectedCharacterTypes;

    void AddSelectedType(ECharacterType Type);

    UPROPERTY(BlueprintAssignable)
    FOnCharacterTypeSelected OnCharacterTypeSelected;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    int32 ClearedMapCount;

    void OnMapCleared();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guide")
    ULevel_DataAsset* LevelGuideDataAsset;

    UFUNCTION(BlueprintCallable)
    FText GetCurrentGuideText() const;

    UPROPERTY(Replicated , BlueprintReadWrite)
    bool bIsInGameStarted = false;

    void StartGame();

    float GetPlayTime() const;

    UPROPERTY(Replicated, BlueprintReadWrite)
    float GameStartTime = -1.f;

    UPROPERTY(Replicated, BlueprintReadWrite)
    float ClearPlayTime = -1.f;

    FString ClearTimeString;

protected:

    UFUNCTION()
    void OnRep_SelectedTypes();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
