
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Server/ServerTypes.h"
#include "InGameHUDWidget.generated.h"

class UTextBlock;
class UWidget;

UCLASS()
class PROJECT_14_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateEndReason(EGameEndReason Reason);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_PlayTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Tutorial;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_ClearTime;


	virtual void NativeTick(const FGeometry& MyGeometry,float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateUI(EGameEndReason Reason);

	FText ConvertSecondsToText(float Time) const;
};
