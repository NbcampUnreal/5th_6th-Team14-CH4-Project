#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerCharacter.h"
#include "GameManager/BaseGameStateBase.h"
#include "UI/MH/Level_DataAsset.h"
#include "ProjectGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterTypeSelected);


UCLASS()
class PROJECT_14_API AProjectGameStateBase : public ABaseGameStateBase
{
	GENERATED_BODY()
	
    AProjectGameStateBase();
public:
    virtual void Tick(float DeltaTime) override;
    virtual TArray<APlayerState*> GetPlayersForChat(APlayerState* SenderPS) override;

    UPROPERTY(ReplicatedUsing = OnRep_SelectedTypes)
    TArray<ECharacterType> SelectedCharacterTypes;

    void AddSelectedType(ECharacterType Type);

    UPROPERTY(BlueprintAssignable)
    FOnCharacterTypeSelected OnCharacterTypeSelected;

    UPROPERTY(BlueprintReadOnly, Replicated)
    float PlayTime = 0.f;

    UPROPERTY(BlueprintReadOnly)
    int32 ClearedMapCount = 0;

    void OnMapCleared();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guide")
    ULevel_DataAsset* LevelGuideDataAsset;

    UFUNCTION(BlueprintCallable)
    FText GetCurrentGuideText() const;

protected:

    UFUNCTION()
    void OnRep_SelectedTypes();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
