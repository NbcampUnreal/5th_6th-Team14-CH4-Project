#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Player/PlayerCharacter.h"
#include "ProjectGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API UProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Character Select")
	ECharacterType SelectedCharacterType;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FString PlayerName = TEXT("Player");
};
