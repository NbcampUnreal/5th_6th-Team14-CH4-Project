#include "UI/MH/InGameHUDWidget.h"
#include "Components/TextBlock.h"
#include "GameManager/ProjectGameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UInGameHUDWidget::NativeTick(const FGeometry& MyGeometry,float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AProjectGameStateBase* GS =
		Cast<AProjectGameStateBase>(UGameplayStatics::GetGameState(this));

	if (!GS || !Text_PlayTime)
		return;

	const float PlayTime = GS->PlayTime;
	Text_PlayTime->SetText(ConvertSecondsToText(PlayTime));
}

FText UInGameHUDWidget::ConvertSecondsToText(float Time) const
{
	int32 TotalSeconds = FMath::FloorToInt(Time);

	int32 Hours = TotalSeconds / 3600;
	int32 Minutes = (TotalSeconds % 3600) / 60;
	int32 Seconds = TotalSeconds % 60;

	if (Hours > 0)
	{
		return FText::FromString(FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
	}
}