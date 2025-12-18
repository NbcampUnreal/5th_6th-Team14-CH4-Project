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
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(ReplicatedUsing = OnRep_SelectedTypes)
    TArray<ECharacterType> SelectedCharacterTypes;

    void AddSelectedType(ECharacterType Type);

    UPROPERTY(BlueprintAssignable)
    FOnCharacterTypeSelected OnCharacterTypeSelected;

    UPROPERTY(BlueprintReadOnly, Replicated)
    float PlayTime = 0.f;

protected:

    UFUNCTION()
    void OnRep_SelectedTypes();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
