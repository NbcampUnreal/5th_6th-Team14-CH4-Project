#include "UI/CharacterSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCtr.h"

void UCharacterSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_WeakJump) Button_WeakJump->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnWeakJumpClicked);
    if (Button_StrongPush) Button_StrongPush->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnStrongPushClicked);

    if (AProjectGameStateBase* GS = GetWorld()->GetGameState<AProjectGameStateBase>())
    {
        GS->OnCharacterTypeSelected.AddDynamic(this, &UCharacterSelectWidget::UpdateCharacterButtons);
    }

    UpdateCharacterButtons();
}

void UCharacterSelectWidget::UpdateCharacterButtons()
{
    if (AProjectGameStateBase* GS = GetWorld()->GetGameState<AProjectGameStateBase>())
    {
        if (!IsInViewport()) return;

        for (ECharacterType Type : GS->SelectedCharacterTypes)
        {
            if (Type == ECharacterType::WeakJump && Button_WeakJump)
                Button_WeakJump->SetVisibility(ESlateVisibility::Collapsed);
            else if (Type == ECharacterType::StrongPush && Button_StrongPush)
                Button_StrongPush->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UCharacterSelectWidget::OnWeakJumpClicked()
{
    if (APlayerCtr* PC = Cast<APlayerCtr>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->Server_SelectCharacterType(ECharacterType::WeakJump);
    }
}

void UCharacterSelectWidget::OnStrongPushClicked()
{
    if (APlayerCtr* PC = Cast<APlayerCtr>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->Server_SelectCharacterType(ECharacterType::StrongPush);
    }
}
