// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TitleLayout.generated.h"

class UButton;
class UEditableText;

/**
 * 
 */
UCLASS()
class PROJECT_14_API UUW_TitleLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_TitleLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget)) 
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USLobbyLevelUI, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> ServerIPEditableText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USLobbyLevelUI, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> PlayerNameEditableText;
    
};
