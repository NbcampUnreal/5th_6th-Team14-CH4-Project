
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerCharacter.h"
#include "GameManager/ProjectGameState.h"
#include "CharacterSelectWidget.generated.h"

class UButton;

UCLASS()
class PROJECT_14_API UCharacterSelectWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Button_WeakJump;

    UPROPERTY(meta = (BindWidget))
    UButton* Button_StrongPush;

protected:
    UFUNCTION()
    void UpdateCharacterButtons();

    UFUNCTION()
    void OnWeakJumpClicked();

    UFUNCTION()
    void OnStrongPushClicked();
};