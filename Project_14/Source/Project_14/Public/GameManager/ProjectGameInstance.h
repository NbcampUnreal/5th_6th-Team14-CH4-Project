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
	virtual void Init() override;

	
	//Character
	UPROPERTY(BlueprintReadWrite, Category = "Character Select")
	ECharacterType SelectedCharacterType;

	//User Setting
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FString PlayerName = TEXT("Player");
};
