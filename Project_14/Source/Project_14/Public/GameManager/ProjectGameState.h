#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Player/PlayerCharacter.h"
#include "ProjectGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterTypeSelected);

UCLASS()
class PROJECT_14_API AProjectGameState : public AGameState
{
	GENERATED_BODY()
	
public:
    AProjectGameState();

    UPROPERTY(ReplicatedUsing = OnRep_SelectedTypes)
    TArray<ECharacterType> SelectedCharacterTypes;

    void AddSelectedType(ECharacterType Type);

    UPROPERTY(BlueprintAssignable)
    FOnCharacterTypeSelected OnCharacterTypeSelected;

protected:
    UFUNCTION()
    void OnRep_SelectedTypes();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;
};
