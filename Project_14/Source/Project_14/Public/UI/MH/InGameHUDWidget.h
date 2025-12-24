
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_14_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_PlayTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Tutorial;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_ClearTime;

	virtual void NativeTick(const FGeometry& MyGeometry,float InDeltaTime) override;

	FText ConvertSecondsToText(float Time) const;
};
