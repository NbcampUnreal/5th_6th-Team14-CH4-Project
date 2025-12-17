#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerCharacter.h"
#include "ProjectGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterTypeSelected);


UCLASS()
class PROJECT_14_API AProjectGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
    AProjectGameStateBase();
public:

    UPROPERTY(ReplicatedUsing = OnRep_SelectedTypes)
    TArray<ECharacterType> SelectedCharacterTypes;

    void AddSelectedType(ECharacterType Type);

    UPROPERTY(BlueprintAssignable)
    FOnCharacterTypeSelected OnCharacterTypeSelected;

protected:

    UFUNCTION()
    void OnRep_SelectedTypes();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
