#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "ProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API AProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AProjectGameModeBase();
		
public:
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<APlayerCharacter> PlayerCharacterClass;

	UFUNCTION(BlueprintCallable)
	void SpawnSelectedCharacter(APlayerController* PC, ECharacterType Type);
};
